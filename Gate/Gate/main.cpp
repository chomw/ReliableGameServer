#include "Common.h"
#include "App.h"
#include "glog/logging.h"

int main(int argc, char* argv[])
{
	App::instance().create("Gate", "./", rgs::Delay::SECOND_MILLI * 1);
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
			LOG(INFO) << "COUNT : " << App::instance().count;
		}

		if (!strcmp(szExit, "clear"))
		{
			App::instance().count = 0;
			system("cls");
		}
	}
	
	App::instance().destroy();
	return 0;
}

