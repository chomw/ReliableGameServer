#include "stdafx.h"
#include "Disconnector.h"
#include "../Socket.h"

using namespace rgs::io::proactor;

void Disconnector::procEvent(Act* act, DWORD bytes)
{
	assert(act);

	rgs::io::Socket* socket = act->socket_;
	assert(socket);
	
	socket->complete(rgs::io::IoEvent::IO_EVENT_DISCONNECTED);
}

void Disconnector::procError(Act* act, DWORD error)
{
	assert(act);

	rgs::io::Socket* socket = act->socket_;
	assert(socket);

	socket->createRowSocket();
	socket->complete(rgs::io::IoEvent::IO_EVENT_DISCONNECTED);
}