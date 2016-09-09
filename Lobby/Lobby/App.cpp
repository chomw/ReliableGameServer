#include "App.h"
#include "LobbyHandler.h"

#include "Common.h"
#include "helper/Logger.h"
#include "io/Connection.h"
#include "io/Session.h"

std::unique_ptr<App> App::instance_ = nullptr;

App& App::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<App>();
	}
	return *instance_.get();
}

void App::create(rgs::Core core, const std::string& appName, const std::string& version, int loggingLevel, bool consoleLog, DWORD updateInterval)
{
	rgs::Application::create(core, appName, version, loggingLevel, consoleLog, updateInterval);

	std::shared_ptr<rgs::io::Listener> connection = std::make_shared<rgs::io::Listener>(8000);
	connection->initialize(LobbyHandler::createProtocol(), 5000, 15000);
	addConnection("frontend", connection);
}

void App::update()
{
}