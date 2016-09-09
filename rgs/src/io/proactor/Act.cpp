#include "stdafx.h"
#include "Act.h"
#include "Actor.h"

using namespace rgs::io::proactor;

Act::Act() : 
actor_(nullptr), 
socket_(nullptr)
{
	hEvent = NULL;
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
}

void Act::Complete(DWORD bytes)
{
	actor_->procEvent(this, bytes);
}

void Act::Error(DWORD error)
{
	actor_->procError(this, error);
}

void Act::initialize(Actor* actor, rgs::io::Socket* socket)
{
	actor_ = actor;
	socket_ = socket;
}