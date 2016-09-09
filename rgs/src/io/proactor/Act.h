#pragma once
#include <windows.h>
#include <winsock2.h>

namespace rgs
{
	namespace io
	{
		class Socket;
		namespace proactor
		{
			class Actor;
			class Act : public OVERLAPPED
			{
			public:
				Act();

				void Complete(DWORD bytes);
				void Error(DWORD error);

				void initialize(Actor* actor, rgs::io::Socket* socket);

			public:
				Actor* actor_;
				rgs::io::Socket* socket_;
			};
		}
	}
}