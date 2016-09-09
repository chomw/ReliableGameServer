#pragma once
#include "Actor.h"
#include <Windows.h>

namespace rgs
{
	namespace io
	{
		namespace proactor
		{
			class Sender : public Actor
			{
			public:
				virtual void procEvent(Act* act, DWORD bytes) override;
				virtual void procError(Act* act, DWORD error) override;
			};
		}
	}
}