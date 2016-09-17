#pragma once
#include <Windows.h>
#include <process.h>
#include <assert.h>
#include <atomic>

namespace rgs
{
	namespace thread
	{
		/**
		@namespace sync
		@brief multi-threading 동기화 관련 네임스페이스
		@since 1.0.0
		*/
		namespace sync
		{
			class Lock
			{
			public:
				virtual ~Lock();

				virtual void enter() = 0;
				virtual void leave() = 0;
			};

			//기본적인 동기화 클래스
			class SimpleLock : public Lock
			{
			public:
				SimpleLock();
				~SimpleLock();

				virtual void enter() override;
				virtual void leave() override;

			private:
				CRITICAL_SECTION sync_obj;
			};

			class scope
			{
			public:
				explicit scope(Lock* _lock) : lock(_lock)
				{
					assert(this->lock);
					lock->enter();
				}

				~scope()
				{
					lock->leave();
				}

			private:
				Lock* lock;
			};

			//실행 순서를 동기화하는 클래스
			class ExecutionOrderLock
			{
			public:
				ExecutionOrderLock()
				{
					hEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
				}
				~ExecutionOrderLock()
				{
					if (hEvent)
					{
						::CloseHandle(hEvent);
					}
				}
				void wait()
				{
					//hEvent가 Signaled상태면 즉시 리턴되고, Non-signaled상태면 대기한다.
					::WaitForSingleObject(hEvent, INFINITE);
				}

				void lock()
				{
					if (lockCount_ == 0)
					{
						//hEvent를 Non-signaled상태로 만든다.
						::ResetEvent(hEvent);
					}
					lockCount_++;
				}

				void unlock()
				{
					lockCount_--;
					if (lockCount_ == 0)
					{
						//hEvent를 Signaled상태로 만든다.
						::SetEvent(hEvent);
					}
				}

			private:
				std::atomic<int> lockCount_ = 0;
				HANDLE hEvent = 0;
			};

			//thread-safe하지 않아도 되는 영역의 동기화
			class ExecutionAroundScope
			{
			public:
				explicit ExecutionAroundScope(ExecutionOrderLock& pointLock, ExecutionOrderLock& aroundLock) : pointLock_(pointLock), aroundLock_(aroundLock)
				{
					pointLock_.wait();
					aroundLock_.lock();
				}

				~ExecutionAroundScope()
				{
					aroundLock_.unlock();
				}

			private:
				ExecutionOrderLock& pointLock_;
				ExecutionOrderLock& aroundLock_;
			};

			//thread-safe해야만 하는 영역의 동기화
			class ExecutionPointScope
			{
			public:
				explicit ExecutionPointScope(ExecutionOrderLock& pointLock, ExecutionOrderLock& aroundLock) : pointLock_(pointLock), aroundLock_(aroundLock)
				{
					pointLock_.lock();
					aroundLock_.wait();
				}

				~ExecutionPointScope()
				{
					pointLock_.unlock();
				}

			private:
				ExecutionOrderLock& pointLock_;
				ExecutionOrderLock& aroundLock_;
			};
		}
	}
}

