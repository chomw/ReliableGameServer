#include "stdafx.h"
#include "../Socket.h"

#include "Receiver.h"

using namespace rgs::io::proactor;

void Receiver::procEvent(Act* act, DWORD bytes)
{
	assert(act);

	rgs::io::Socket* socket = act->socket_;
	assert(socket);

	if (bytes <= 0)
	{
		socket->disconnect();
	}
	else
	{
		if (socket->receive(bytes) == true)
		{
			socket->startReceive();
		}
		else
		{
			socket->disconnect();
		}
	}
}

void Receiver::procError(Act* act, DWORD error)
{
	assert(act);
	rgs::io::Socket* socket = act->socket_;
	assert(socket);

	socket->disconnect();
}