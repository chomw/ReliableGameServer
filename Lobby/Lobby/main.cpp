#include "Common.h"
#include "App.h"
#include "helper/Logger.h"

int main(int argc, char* argv[])
{
	App::instance().create(rgs::Core::SINGLE, "Lobby", "1.0.0", severity_level::debug, false, rgs::Delay::SECOND_MILLI * 1);
	App::instance().start();

	while (true)
	{
		char szExit[16] = "\0";
		scanf("%s", szExit);

		if (!strcmp(szExit, "exit"))
		{
			App::instance().stop();
			break;
		}
		if (!strcmp(szExit, "show"))
		{
			RGS_LOG(info) << "COUNT : " << App::instance().count;
		}
	}
	
	App::instance().destroy();
	return 0;
}

