#include "stdafx.h"
#include "Acceptor.h"
#include "../Socket.h"

using namespace rgs::io::proactor;

void Acceptor::procEvent(Act* act, DWORD bytes)
{
	assert(act);

	Socket* socket = act->socket_;
	assert(socket);
	
	socket->complete(rgs::io::IoEvent::IO_EVENT_CONNECTED);
	socket->startReceive();
}

void Acceptor::procError(Act* act, DWORD error)
{
	assert(act);

	Socket* socket = act->socket_;
	assert(socket);

	socket->createRowSocket();
	socket->listening();
}