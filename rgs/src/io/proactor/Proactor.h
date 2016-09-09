#pragma once
#include <vector>

#include "Act.h"
#include <windows.h>
#include <process.h>

namespace rgs
{
	namespace io
	{
		namespace proactor
		{
			class Proactor
			{
			public:
				static const int ExitCode;

				static UINT WINAPI callback(void* pProactor);

				void begin(unsigned int size);
				void end();

				void registerToIocp(::HANDLE handle);
				void registerToIocp(::SOCKET socket);

				void postPrivateEvent(DWORD pId, Act* actor);
				void procEvent();

			private:
				::HANDLE iocp_;
				std::vector<::HANDLE> threads;
			};
		}
	}
}