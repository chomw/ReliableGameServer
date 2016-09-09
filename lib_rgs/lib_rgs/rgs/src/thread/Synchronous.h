#pragma once
#include <Windows.h>
#include <process.h>
#include <assert.h>
#include <atomic>

namespace rgs
{
	namespace thread
	{
		namespace sync
		{
			class Lock
			{
			public:
				virtual ~Lock();

				virtual void enter() = 0;
				virtual void leave() = 0;
			};

			//�⺻���� ����ȭ Ŭ����
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

			//���� ������ ����ȭ�ϴ� Ŭ����
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
					//hEvent�� Signaled���¸� ��� ���ϵǰ�, Non-signaled���¸� ����Ѵ�.
					::WaitForSingleObject(hEvent, INFINITE);
				}

				void lock()
				{
					if (lockCount_ == 0)
					{
						//hEvent�� Non-signaled���·� �����.
						::ResetEvent(hEvent);
					}
					lockCount_++;
				}

				void unlock()
				{
					lockCount_--;
					if (lockCount_ == 0)
					{
						//hEvent�� Signaled���·� �����.
						::SetEvent(hEvent);
					}
				}

			private:
				std::atomic<int> lockCount_ = 0;
				HANDLE hEvent = 0;
			};

			//thread-safe���� �ʾƵ� �Ǵ� ������ ����ȭ
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

			//thread-safe�ؾ߸� �ϴ� ������ ����ȭ
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

