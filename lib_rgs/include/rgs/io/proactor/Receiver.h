#pragma once
#include <memory>

#include "Actor.h"
#include <windows.h>

namespace rgs
{
	namespace io
	{
		namespace proactor
		{
			class Receiver : public Actor
			{
			public:
				virtual void procEvent(Act* act, DWORD bytes) override;
				virtual void procError(Act* act, DWORD error) override;
			};
		}
	}
}