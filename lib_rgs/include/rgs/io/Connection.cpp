#include "stdafx.h"
#include "Connection.h"
#include "NetworkEngine.h"
#include "Session.h"

#include "../protocol/Protocol.h"

#include "glog/logging.h"

using namespace rgs::io;

Connection::Connection(int port) : port_(port)
{
	registerHandler(rgs::io::IoEvent::IO_EVENT_CONNECTED, std::bind(&Connection::connected, this, std::placeholders::_1));
	registerHandler(rgs::io::IoEvent::IO_EVENT_DISCONNECTED, std::bind(&Connection::disconnected, this, std::placeholders::_1));
}

void Connection::update()
{
	//SINGLE 모드에서 io 처리
	if (NetworkEngine::instance().getCore() == rgs::Core::SINGLE)
	{
		Connection::IoEvent element;
		while (ioEventQueue_.empty() == false)
		{
			if (ioEventQueue_.try_pop(element) == true)
			{
				handlers_.call((int)element.ioEvent, std::move(element.session));
			}
		}

		dispatch();
	}
}

std::shared_ptr<rgs::io::Session>
Connection::getSession()
{
	return nullptr;
}

unsigned int Connection::sessionSize()const
{
	return sessionSize_;
}

unsigned int Connection::connectedSize()const
{
	return connectedSize_;
}

unsigned int Connection::waitingSize()const
{
	long waitSize = sessionSize_ - connectedSize_;
	return waitSize < 0 ? 0 : (unsigned int)waitSize;
}


void Connection::complete(rgs::io::IoEvent ioEvent, std::shared_ptr<rgs::io::Session> session)
{
	if (NetworkEngine::instance().getCore() == rgs::Core::MULTI)
	{
		handlers_.call((int)ioEvent, std::move(session));
	}
	else
	{
		ioEventQueue_.push(Connection::IoEvent(ioEvent, session));
	}
}

void Connection::dispatch()
{
	for (auto iterator : connectedSessions_)
	{
		auto session = iterator.second;
		session->dispatch();
	}
}

void Connection::connected(std::shared_ptr<rgs::io::Session> session)
{
	if (NetworkEngine::instance().getCore() == rgs::Core::SINGLE)
	{
		connectedSessions_[session->id()] = session;
	}

	connectedSize_++;	
}

void Connection::disconnected(std::shared_ptr<rgs::io::Session> session)
{
	if (NetworkEngine::instance().getCore() == rgs::Core::SINGLE)
	{
		connectedSessions_.erase(session->id());
	}

	connectedSize_--;
}

Listener::Listener(int port) : Connection(port) {}

void Listener::complete(rgs::io::IoEvent ioEvent, std::shared_ptr<rgs::io::Session> session)
{
	Connection::complete(ioEvent, session);

	if (ioEvent == rgs::io::IoEvent::IO_EVENT_CONNECTED)
	{
		if (waitingSize() < capacity_)
		{
			createSession();
		}
	}

	if (ioEvent == rgs::io::IoEvent::IO_EVENT_DISCONNECTED)
	{
		session->listening();
	}
}

bool
Listener::initialize(std::shared_ptr<rgs::protocol::Protocol> protocol, unsigned int capacity, unsigned int maxConnection)
{
	assert(protocol.get());

	if (protocol == nullptr)
	{
		LOG(ERROR) << "Connector::initialize Error : " << rgs::Error::ERROR_NULL_DATA;
		return false;
	}

	protocol_ = protocol;
	capacity_ = capacity;
	maxConnection_ = maxConnection;

	for (unsigned int i = 0; i < capacity_; ++i)
	{
		createSession();
	}

	return true;
}

std::shared_ptr<Session>
Listener::createSession()
{
	if (sessionSize() >= maxConnection_)
	{
		return nullptr;
	}

	std::shared_ptr<Session> session = nullptr;

	session = std::make_shared<Session>(this);
	
	session->initialize(protocol_, port_);

	session->listening();

	sessions_.push_back(session);
	sessionSize_++;

	return session;
}

Connector::Connector(const rgs::io::IPAddress& endPointIp) :
	Connection(),
	endPointIp_(std::make_shared<rgs::io::IPAddress>(endPointIp)) {}

bool
Connector::initialize(std::shared_ptr<rgs::protocol::Protocol> protocol)
{
	assert(protocol.get());

	if (protocol == nullptr)
	{
		LOG(ERROR) << "Connector::initialize Error : " << rgs::Error::ERROR_NULL_DATA;
		return false;
	}

	protocol_ = protocol;
	capacity_ = 0;
	maxConnection_ = 0;

	registerHandler(rgs::io::IoEvent::IO_EVENT_DISCONNECTED, std::bind(&Connector::returnSession, this, std::placeholders::_1));

	return true;
}

std::shared_ptr<rgs::io::Session>
Connector::getSession()
{
	std::shared_ptr<Session> session = createSession();

	if (session->bind() == false)
	{
		LOG(ERROR) << "Connecter::getSession() Error";

		returnSession(session);

		return nullptr;
	}

	if (session->connect(endPointIp_->host, endPointIp_->port) == false)
	{
		LOG(ERROR) << "Connecter::getSession() Error";

		returnSession(session);

		return nullptr;
	}

	return session;
}

unsigned int 
Connector::waitingSize()const
{
	return waitingSessions_.unsafe_size();
}

std::shared_ptr<Session>
Connector::createSession()
{
	std::shared_ptr<Session> session = nullptr;
	
	if (waitingSessions_.try_pop(session) == false)
	{
		session = std::make_shared<Session>(this);

		session->initialize(protocol_);

		sessions_.push_back(session);
		sessionSize_++;
	}
	else
	{
		session->initialize();
	}

	return session;
}

void
Connector::returnSession(std::shared_ptr<rgs::io::Session> session)
{
	assert(session);

	session->pairSession = nullptr;

	waitingSessions_.push(session);
}