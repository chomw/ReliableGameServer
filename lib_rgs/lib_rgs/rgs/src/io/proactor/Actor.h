#pragma once
#include <windows.h>

namespace rgs
{
	namespace io
	{
		namespace proactor
		{
			class Act;
			class Actor
			{
			public:
				Actor(){}
				virtual ~Actor(){}

				virtual void procEvent(Act* act, DWORD bytes) = 0;
				virtual void procError(Act* act, DWORD error) = 0;
			};
		}
	}
}