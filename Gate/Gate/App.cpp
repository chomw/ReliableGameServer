#include "App.h"
#include "GateHandler.h"

#include "Common.h"

#include "io/Connection.h"
#include "io/Session.h"

#include "protocol/JsonProtocol.h"

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

std::vector<std::shared_ptr<rgs::io::Session>> sessions;
void App::create(rgs::Core core, const std::string& appName, const std::string& logPath, DWORD updateInterval)
{
	rgs::Application::create(core, appName, logPath, updateInterval);

	std::shared_ptr<rgs::io::Listener> connection = std::make_shared<rgs::io::Listener>(7000);
	connection->initialize(GateHandler::createProtocol(), 5000, 15000);
	addConnection("frontend", connection);

	std::shared_ptr<rgs::io::Connector> backendConnection = std::make_shared<rgs::io::Connector>(rgs::io::IPAddress("127.0.0.1", 8000));
	backendConnection->initialize(GateHandler::createProtocol());
	addConnection("backend", backendConnection);

	connection->registerHandler(rgs::io::IoEvent::IO_EVENT_CONNECTED, [backendConnection](std::shared_ptr<rgs::io::Session> session) {

		session->useKeepAlive(true);

		auto pairSession = backendConnection->getSession();

		if (pairSession)
		{
			session->pairSession = pairSession;
			pairSession->pairSession = session;
		}

	});

	connection->registerHandler(rgs::io::IoEvent::IO_EVENT_DISCONNECTED, [backendConnection](std::shared_ptr<rgs::io::Session> session) {

		if (session->pairSession)
		{
			session->pairSession->disconnect();
			session->pairSession = nullptr;
		}
	});
}
void App::update()
{
	/*std::shared_ptr<rgs::io::Connection> frontend = getConnection("frontend");
	LOG(ERROR) << "Frontend Session Size : " << frontend->sessionSize() << ", Connected Size : " << frontend->connectedSize() << ", Wait Size : " << frontend->waitingSize();

	std::shared_ptr<rgs::io::Connector> backend = std::dynamic_pointer_cast<rgs::io::Connector>(getConnection("backend"));
	LOG(ERROR) << "Backend Session Size : " << backend->sessionSize() << ", Connected Size : " << backend->connectedSize() << ", Wait Size : " << backend->waitingSize();*/
}