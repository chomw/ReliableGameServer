#include "stdafx.h"
#include "Proactor.h"

using namespace rgs::io::proactor;

const int Proactor::ExitCode = -1;

UINT WINAPI Proactor::callback(void* pProactor)
{
	static_cast<Proactor*>(pProactor)->procEvent();
	return 0;
}

void Proactor::begin(unsigned int size)
{
	int threadCount = size;
	iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, threadCount);

	threads.clear();
	for (int i = 0; i < threadCount; i++)
	{
		unsigned int threadid;
		HANDLE hThread = (HANDLE)::_beginthreadex(NULL, 0, Proactor::callback, (void *)this, 0, &threadid);
		threads.push_back(hThread);
	}
}

void Proactor::end()
{
	for (int i = 0; i < (int)threads.size(); ++i)
	{
		postPrivateEvent(Proactor::ExitCode, NULL);
	}

	for (auto& thrd : threads)
	{
		::WaitForSingleObject(thrd, INFINITE);
		::CloseHandle(thrd);
	}
	threads.clear();

	if (iocp_)
	{
		::CloseHandle(iocp_);
	}
}


void Proactor::registerToIocp(::HANDLE handle)
{
	::CreateIoCompletionPort(handle, iocp_, 0, 0);
}

void Proactor::registerToIocp(::SOCKET socket)
{
	registerToIocp(reinterpret_cast<HANDLE>(socket));
}

void Proactor::postPrivateEvent(DWORD pId, Act* act)
{
	::PostQueuedCompletionStatus(iocp_, 0, pId, act);
}

void Proactor::procEvent()
{
	while (TRUE)
	{
		DWORD bytes;
		DWORD completionKey;

		OVERLAPPED* overlapped = NULL;

		BOOL status = ::GetQueuedCompletionStatus(iocp_, &bytes, static_cast<PULONG_PTR>(&completionKey), &overlapped, INFINITE);

		if (status == TRUE)
		{
			if (completionKey == Proactor::ExitCode)
			{
				return;
			}

			if (overlapped == NULL)
			{
				continue;
			}
			Act* act = static_cast<Act*>(overlapped);
			act->Complete(bytes);
		}
		else
		{
			DWORD error = ::WSAGetLastError();
			
			if (overlapped != NULL)
			{
				Act* act = static_cast<Act*>(overlapped);
				act->Error(error);
				continue;
			}
			continue;
		}
	}
}