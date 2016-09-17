#include "stdafx.h"
#include "Connection.h"
#include "NetworkEngine.h"
#include "Session.h"

#include "../protocol/Protocol.h"

#include "glog/logging.h"

using namespace rgs::io;

void Connection::update()
{
	Connection::ConnectionEvent element;
	while (connectionEventQueue_.empty() == false)
	{
		if (connectionEventQueue_.try_pop(element) == true)
		{
			//Ŀ�ؼ� �̺�Ʈ �ڵ鷯�� ȣ���Ѵ�. (���� ����)
			handlers_.call(
				(int)element.connectionEvent, 
				element.session
			);

			//Ŀ�ؼ� �̺�Ʈ�� ���������� ó���Ѵ�. (���� ����)
			if (element.connectionEvent == rgs::io::ConnectionEvent::CONNECTED)
			{
				connected(element.session);

				//���� ���� ���� ��Ŷ ó���� �����Ѵ�.
				connectedSessions_[element.session->id()] = element.session;
				connectedSize_++;
			}
			else
			{
				element.session->dispatch();
				
				connectedSessions_.erase(element.session->id());
				connectedSize_--;
				
				//��Ŷ ó�� ���� ���� ���� ó���� �Ѵ�.
				disconnected(element.session);
			}
		}
	}

	dispatch();
}

unsigned int Connection::connectedSize()const
{
	return connectedSize_;
}

void Connection::complete(rgs::io::ConnectionEvent connectionEvent, std::shared_ptr<rgs::io::Session> session)
{
	connectionEventQueue_.push(Connection::ConnectionEvent(connectionEvent, session));
}

void Connection::connected(std::shared_ptr<Session> session){}

void Connection::disconnected(std::shared_ptr<Session> session){}

void Connection::dispatch()
{
	for (auto iterator : connectedSessions_)
	{
		auto session = iterator.second;
		session->dispatch();
	}
}


bool
Listener::initiate(std::shared_ptr<rgs::io::CreateSession> createSession, int port)
{
	assert(createSession);

	if (createSession == nullptr)
	{
		LOG(ERROR) << "Connector::initiate Error : " << rgs::Error::ERROR_NULL_DATA;
		return false;
	}

	createSession_ = createSession;
	port_ = port;

	if (capacity_ <= 0)
	{
		capacity_ = Listener::CAPACITY;
	}

	if (maxSize_ <= 0)
	{
		maxSize_ = Listener::MAX_SIZE;
	}

	if (capacity_ > maxSize_)
	{
		capacity_ = maxSize_._My_val;
	}

	keepCapacity();

	return true;
}

void Listener::setCapacity(unsigned int capacity)
{
	capacity_ = capacity;
}

void Listener::setMaxSize(unsigned int maxSize)
{
	maxSize_ = maxSize;
}

void Listener::connected(std::shared_ptr<Session> session) 
{
	keepCapacity();
}

void Listener::disconnected(std::shared_ptr<Session> session) 
{
	session->listening(port_);
}

void Listener::keepCapacity()
{
	assert(createSession_);

	unsigned int waitingSize = size_ - connectedSize();

	for (int i = waitingSize; i <= capacity_; ++i)
	{
		if (size_ >= maxSize_)
		{
			return;
		}

		std::shared_ptr<Session> session = (*createSession_)();

		session->registerHandler(std::bind(&Listener::complete, this, std::placeholders::_1, std::placeholders::_2));

		session->listening(port_);

		sessions_.push_back(session);

		size_++;
	}
}

bool
Connector::initiate(std::shared_ptr<rgs::io::CreateSession> createSession, const rgs::io::IPAddress& endPointIp)
{
	assert(createSession);

	if (createSession == nullptr)
	{
		LOG(ERROR) << "Connector::initiate Error : " << rgs::Error::ERROR_NULL_DATA;
		return false;
	}

	createSession_ = createSession;

	endPointIp_ = std::make_shared<rgs::io::IPAddress>(endPointIp);

	return true;
}

std::shared_ptr<rgs::io::Session>
Connector::getSession()
{
	std::shared_ptr<Session> session = nullptr;

	///��� ���� ť���� ������ ��´�.
	if (waitingSessions_.try_pop(session) == true)
	{
		session->initialize();
	}

	///���� ���ϸ� ���� �����Ѵ�.
	if(session == nullptr)
	{
		assert(createSession_);

		session = (*createSession_)();

		session->registerHandler(std::bind(&Connector::complete, this, std::placeholders::_1, std::placeholders::_2));
	}

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

void Connector::disconnected(std::shared_ptr<Session> session)
{
	returnSession(session);
}

void
Connector::returnSession(std::shared_ptr<rgs::io::Session> session)
{
	assert(session);

	session->pairSession = nullptr;

	waitingSessions_.push(session);
}