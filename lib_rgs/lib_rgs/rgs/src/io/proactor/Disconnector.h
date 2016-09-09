#pragma once 
#include <windows.h>
#include "Actor.h"

namespace rgs
{
	namespace io
	{
		namespace proactor
		{
			class Disconnector : public Actor
			{
			public:
				virtual void procEvent(Act* act, DWORD bytes) override;
				virtual void procError(Act* act, DWORD error) override;
			};
		}
	}
}