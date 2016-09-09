#include "stdafx.h"
#include "Synchronous.h"

using namespace rgs::thread::sync;

rgs::thread::sync::Lock::~Lock() {}

SimpleLock::SimpleLock()
{
	InitializeCriticalSection(&sync_obj);
}

SimpleLock::~SimpleLock()
{
	DeleteCriticalSection(&sync_obj);
}

void SimpleLock::enter()
{
	EnterCriticalSection(&sync_obj);
}

void SimpleLock::leave()
{
	LeaveCriticalSection(&sync_obj);
}