#include "App.h"
#include "LobbyHandler.h"

#include "Common.h"
#include "io/Connection.h"
#include "io/Session.h"

#include "glog/logging.h"

std::unique_ptr<App> App::instance_ = nullptr;

App& App::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<App>();
	}
	return *instance_.get();
}

void App::create(const std::string& appName, const std::string& logPath, DWORD updateInterval)
{
	rgs::Application::create(appName, logPath, updateInterval);

	auto protocol = LobbyHandler::createProtocol();

	auto createSession = std::make_shared<rgs::io::CreateSession>(protocol);

	std::shared_ptr<rgs::io::Listener> frontend = std::make_shared<rgs::io::Listener>();
	
	frontend->initiate(createSession, 8000);
	
	addConnection("frontend", frontend);
}

void App::update()
{
}