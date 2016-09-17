#pragma once

#include <tuple>
#include "../callback/Callback.h"

namespace rgs
{
	/**
	@namespace thread
	@brief multi-threading 관련 네임스페이스
	@since 1.0.0
	*/
	namespace thread
	{
		template<typename... TArgs>
		class Thread
		{
		private:
			bool initialize();
			bool destroy();

		public:
			Thread();
			~Thread();
			
			void start();
			void stop();

			template<class TFunction>
			void setCallback(TFunction&&);

			template<class TFunction, typename = typename std::enable_if<sizeof...(TArgs) != 0>::type>
			void setCallback(TFunction&&, TArgs&&...);

			void setUpdateInterval(DWORD);

		private:
			static unsigned int WINAPI callback(LPVOID owner);
			void logic();

		private:
			::HANDLE threadHandle_;
			::HANDLE startEvent_;
			::HANDLE destroyEvent_;

			DWORD updateInterval_;
			rgs::cb::Callback<TArgs...> callback_;
		};

		template<typename... TArgs>
		Thread<TArgs...>::Thread() :
			threadHandle_(NULL),
			startEvent_(NULL),
			destroyEvent_(NULL),
			updateInterval_(rgs::Delay::SECOND)
		{
			initialize();
		}

		template<typename... TArgs>
		Thread<TArgs...>::~Thread()
		{
			destroy();
		}

		template<typename... TArgs>
		bool Thread<TArgs...>::initialize()
		{
			startEvent_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
			if (startEvent_ == NULL)
			{
				destroy();
				return false;
			}

			destroyEvent_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			if (destroyEvent_ == NULL)
			{
				destroy();
				return false;
			}

			threadHandle_ = (HANDLE) ::_beginthreadex(NULL, 0, Thread<TArgs...>::callback, this, 0, NULL);
			if (threadHandle_ == NULL)
			{
				destroy();
				return false;
			}

			return true;
		}

		template<typename... TArgs>
		bool Thread<TArgs...>::destroy()
		{
			::SetEvent(destroyEvent_);

			if (threadHandle_ != NULL)
			{
				::WaitForSingleObject(threadHandle_, INFINITE);
				::CloseHandle(threadHandle_);
			}
			
			if (destroyEvent_ != NULL)
			{
				::CloseHandle(destroyEvent_);
			}

			if (startEvent_ != NULL)
			{
				::CloseHandle(startEvent_);
			}

			return true;
		}

		template<typename... TArgs>
		void Thread<TArgs...>::start()
		{
			if (startEvent_ != NULL)
			{
				::SetEvent(startEvent_);
			}
		}

		template<typename... TArgs>
		void Thread<TArgs...>::stop()
		{
			if (startEvent_ != NULL)
			{
				::ResetEvent(startEvent_);
			}
		}

		template<typename... TArgs>
		template<class TFunction>
		void Thread<TArgs...>::setCallback(TFunction&& function)
		{
			callback_.setCallback(std::forward<TFunction>(function));
		}

		template<typename... TArgs>
		template<class TFunction, typename>
		void Thread<TArgs...>::setCallback(TFunction&& function, TArgs&&... args)
		{
			callback_.setCallback(std::forward<TFunction>(function), std::forward<TArgs>(args)...);
		}

		template<typename... TArgs>
		void Thread<TArgs...>::setUpdateInterval(DWORD updateInterval)
		{
			updateInterval_ = updateInterval;
		}

		template<typename... TArgs>
		unsigned int Thread<TArgs...>::callback(LPVOID owner)
		{
			Thread<TArgs...>* own = static_cast<Thread<TArgs...>*>(owner);
			own->logic();
			return 0;
		}

		template<typename... TArgs>
		void Thread<TArgs...>::logic()
		{
			HANDLE events[3] = { startEvent_, destroyEvent_ };
			while (true)
			{
				Delay delay(updateInterval_);
				DWORD eventId = ::WaitForMultipleObjects(2, events, FALSE, INFINITE);

				switch (eventId)
				{
				case WAIT_OBJECT_0:
					callback_();
					break;
				case WAIT_OBJECT_0 + 1:
					return;
				}
			}
		}
	}
}

