#include "stdafx.h"
#include "Connector.h"
#include "../Socket.h"
using namespace rgs::io::proactor;

void Connector::procEvent(Act* act, DWORD bytes)
{
	assert(act);

	rgs::io::Socket* socket = act->socket_;
	assert(socket);

	socket->complete(rgs::io::IoEvent::IO_EVENT_CONNECTED);
	socket->startReceive();
}

void Connector::procError(Act* act, DWORD error)
{
	assert(act);

	rgs::io::Socket* socket = act->socket_;
	assert(socket);

	socket->disconnect();
	socket->complete(rgs::io::IoEvent::IO_EVENT_CONNECTED);
}