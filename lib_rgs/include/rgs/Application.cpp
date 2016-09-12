#include "stdafx.h"
#include "Application.h"
#include "io/NetworkEngine.h"
#include "io/Connection.h"
#include "thread/SingleThread.h"
#include "io/Socket.h"

#include "glog/logging.h"

using namespace rgs;

void Application::mainLoop()
{
	if (rgs::io::NetworkEngine::instance().getCore() == rgs::Core::SINGLE)
	{
		updateConnection();
	}

	update();
}

void Application::create(rgs::Core core, const std::string& appName, const std::string& logPath, DWORD updateInterval)
{
	upDown_ = std::make_shared<rgs::UpDown>(logPath + appName + "-");
	appName_ = appName;
	
	LOG(INFO) << "LOADING " << appName_;

	rgs::io::NetworkEngine::instance().setCore(core);
	rgs::io::NetworkEngine::instance().start();

	mainLoop_ = std::make_shared<rgs::thread::Thread<>>();
	mainLoop_->setCallback(std::bind(&Application::mainLoop, this));
	mainLoop_->setUpdateInterval(updateInterval);
}

void Application::destroy()
{
	mainLoop_.reset();
	
	rgs::io::NetworkEngine::instance().stop();
	
	rgs::io::SocketMonitor::instance().release();

	connections_.clear();

	upDown_.reset();
}

void Application::start()
{
	LOG(INFO) << "START " << appName_;
	mainLoop_->start();
}

void Application::stop()
{
	mainLoop_->stop();

	LOG(INFO) << "STOP " << appName_;
}

void Application::update(){}

void Application::updateConnection()
{
	rgs::thread::sync::scope scope(&lock_);

	for (auto& iterator : connections_)
	{
		auto connection = iterator.second;
		if (connection)
		{
			connection->update();
		}
	}
}

void Application::addConnection(const std::string& name, std::shared_ptr<rgs::io::Connection> connection)
{
	rgs::thread::sync::scope scope(&lock_);
	connections_[name] = connection;
}

void Application::removeConnection(const std::string& name)
{
	rgs::thread::sync::scope scope(&lock_);
	connections_.erase(name);
}

std::shared_ptr<rgs::io::Connection>
Application::getConnection(const std::string& name)
{
	rgs::thread::sync::scope scope(&lock_);
	return connections_[name];
}