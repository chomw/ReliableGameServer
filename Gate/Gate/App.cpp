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
void App::create(const std::string& appName, const std::string& logPath, DWORD updateInterval)
{
	rgs::Application::create(appName, logPath, updateInterval);

	auto protocol = GateHandler::createProtocol();

	auto createSession = std::make_shared<rgs::io::CreateSession>(protocol);

	std::shared_ptr<rgs::io::Listener> frontend = std::make_shared<rgs::io::Listener>();
	frontend->initiate(createSession, 7000);
	addConnection("frontend", frontend);

	std::shared_ptr<rgs::io::Connector> backend = std::make_shared<rgs::io::Connector>();
	backend->initiate(createSession, rgs::io::IPAddress("127.0.0.1", 8000));
	addConnection("backend", backend);

	frontend->registerHandler(rgs::io::ConnectionEvent::CONNECTED, [backend](std::shared_ptr<rgs::io::Session> session) {

		session->setTimeout(30000);

		/*auto pairSession = backend->getSession();

		if (pairSession)
		{
			session->pairSession = pairSession;
			pairSession->pairSession = session;
		}*/

	});

	frontend->registerHandler(rgs::io::ConnectionEvent::DISCONNECTED, [](std::shared_ptr<rgs::io::Session> session) {

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