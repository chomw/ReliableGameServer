#pragma once
#include "../callback/Callback.h"
#include "../thread/Synchronous.h"

namespace rgs
{
	namespace thread
	{
		/****************** ThreadPool *********************/
		template<typename TReturn, typename... TArgs>
		class ThreadPool
		{
		public:
			ThreadPool();

			bool begin(unsigned int threadSize);
			void end();

			void add(const rgs::cb::callback<TReturn, TArgs...>&);

			template<typename Func, typename... TFuncArgs>
			void add(Func&& func, TFuncArgs&&...);

		private:
			static unsigned int WINAPI callback(LPVOID owner);
			void working();

		private:
			enum Behavior { BEHAVIOR_FINISH = 0, BEHAVIOR_WORK = 1 };

		private:
			::HANDLE iocp_;
			::HANDLE startUpEvent_;
			std::vector<::HANDLE> workerThreads_;
		};

		//constructor
		template<typename TReturn, typename... TArgs>
		ThreadPool<TReturn, TArgs...>::ThreadPool() : iocp_(NULL), startUpEvent_(NULL){}

		//methods
		template<typename TReturn, typename... TArgs>
		inline void ThreadPool<TReturn, TArgs...>::add(const rgs::cb::callback<TReturn, TArgs...>& callback)
		{
			auto allocatedCallback = new rgs::cb::callback<TReturn, TArgs...>(callback);
			::PostQueuedCompletionStatus(iocp_, Behavior::BEHAVIOR_WORK, NULL, (LPOVERLAPPED)allocatedCallback);
		}

		template<typename TReturn, typename... TArgs>
		template<typename Func, typename... TFuncArgs>
		inline void ThreadPool<TReturn, TArgs...>::add(Func&& func, TFuncArgs&&... args)
		{
			auto callback = new rgs::cb::callback<TReturn, TArgs...>(func, std::forward<TFuncArgs>(args)...);
			::PostQueuedCompletionStatus(iocp_, Behavior::BEHAVIOR_WORK, NULL, (LPOVERLAPPED)callback);
		}

		template<typename TReturn, typename... TArgs>
		bool ThreadPool<TReturn, TArgs...>::begin(unsigned int threadSize)
		{
			//IOCP 생성
			iocp_ = 0;
			iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
			if (iocp_ == NULL) return false;

			//스레드 생성 시 발생하는 이벤트 오브젝트 생성
			startUpEvent_ = ::CreateEvent(0, FALSE, FALSE, 0);
			if (startUpEvent_ == NULL)
			{
				ThreadPool<TReturn, TArgs...>::end();
				return false;
			}

			//Worker Thread 생성
			for (unsigned int i = 0; i < threadSize; ++i)
			{
				::HANDLE workerThread = (HANDLE)::_beginthreadex(NULL, 0, ThreadPool::callback, this, 0, NULL);
				workerThreads_.push_back(workerThread);

				::WaitForSingleObject(startUpEvent_, INFINITE);
			}

			return true;
		}

		template<typename TReturn, typename... TArgs>
		void ThreadPool<TReturn, TArgs...>::end()
		{
			for (DWORD i = 0; i < workerThreads_.size(); ++i)
			{
				::PostQueuedCompletionStatus(iocp_, (ULONG_PTR)Behavior::BEHAVIOR_FINISH, NULL, NULL);
			}

			for (DWORD i = 0; i < workerThreads_.size(); ++i)
			{
				::WaitForSingleObject(workerThreads_[i], INFINITE);
				::CloseHandle(workerThreads_[i]);
			}

			if (iocp_) ::CloseHandle(iocp_);
			if (startUpEvent_) ::CloseHandle(startUpEvent_);

			workerThreads_.clear();
		}

		//static method : callback() is called by worker thread
		template<typename TReturn, typename... TArgs>
		unsigned int WINAPI ThreadPool<TReturn, TArgs...>::callback(LPVOID owner)
		{
			assert(owner);
			if (!owner)
			{
				return 1;
			}

			reinterpret_cast<ThreadPool<TReturn, TArgs...>*>(owner)->working();

			return 0;
		}

		// method : working() is called by callback()
		template<typename TReturn, typename... TArgs>
		void ThreadPool<TReturn, TArgs...>::working()
		{
			::SetEvent(startUpEvent_);

			while (true)
			{
				DWORD key = 0;
				DWORD behavior = NULL;
				rgs::cb::callback<TReturn, TArgs...>* callback = NULL;

				BOOL result = ::GetQueuedCompletionStatus(
					iocp_,
					&behavior,
					(LPDWORD)&key,
					(LPOVERLAPPED*)&callback,
					INFINITE);

				if (behavior == Behavior::BEHAVIOR_FINISH)
				{
					return;
				}
				
				if (result == false)
				{
					return;
				}

				if (callback != NULL)
				{
					(*callback)();
					delete callback;
				}
			}
		}
	}
}