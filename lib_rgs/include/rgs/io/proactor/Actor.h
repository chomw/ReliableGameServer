#pragma once
#include <windows.h>

namespace rgs
{
	namespace io
	{
		/**
		@namespace proactor
		@brief Proactor 패턴 관련 네임스페이스
		@since 1.0.0
		*/
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