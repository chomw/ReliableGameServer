#include "stdafx.h"

#include <ppl.h>
#include <concurrent_unordered_map.h>

#include "Session.h"
#include "Socket.h"
#include "Connection.h"
#include "NetworkEngine.h"

#include "../protocol/Protocol.h"
#include "../thread/SingleThread.h"

#include "glog/logging.h"

using namespace rgs::io;

unsigned int generateId()
{
	static std::atomic<unsigned int> id = 0;
	return ++id;
}

Session::Session() : 
	id_(0),
	socket_(std::make_shared<Socket>(*this)) {}

bool
Session::initialize()
{
	if (socket_->createRowSocket() == false)
	{
		return false;
	}

	return true;
}

bool 
Session::initialize(std::shared_ptr<rgs::protocol::Protocol> protocol)
{	
	if (socket_->createRowSocket() == false)
	{
		return false;
	}

	protocol_ = protocol;

	//proactor 초기화
	NetworkEngine::instance().registerSocket(socket_);

	return true;
}

bool 
Session::write(const rgs::protocol::RowData& rowData)
{
	return socket_->write(rowData.data, rowData.size);
}

unsigned int
Session::read(const rgs::protocol::RowData& rowData, unsigned int& readBytes)
{
	if (protocol_ == nullptr)
	{
		return 0;
	}

	int error = 0;
	rgs::protocol::Packet* packet = nullptr;
	rgs::protocol::RowData packetRowData = rowData;
	
	while (true)
	{
		if (isConnected() == false)
		{
			LOG(ERROR) << "Session-" << id_ << " Session::read() Error : " << rgs::Error::ERROR_DISCONNECTED_SESSION;
			return rgs::Error::ERROR_DISCONNECTED_SESSION;
		}

		error = protocol_->readPacket(packetRowData, &packet, readBytes);

		if (error)
		{
			LOG(ERROR) << "Session-" << id_ << " Session::read() Error : " << error;
			return error;
		}

		if (readBytes == 0)
		{
			break;
		}

		if (packet == nullptr)
		{
			error = rgs::Error::ERROR_NULL_DATA;
			LOG(ERROR) << "Session-" << id_ << " Session::read() Error : " << error;
			return error;
		}

		packets_.push(packet);

		packetRowData.data += readBytes;
		packetRowData.size -= readBytes;
	}
	
	readBytes = rowData.size - packetRowData.size;

	return 0;
}

bool 
Session::bind()
{
	return socket_->bind();
}

bool 
Session::connect(const std::string& address, int port, const BYTE* data, unsigned int size)
{
	return socket_->connect(address, port, data, size);
}

bool
Session::disconnect()
{
	return socket_->disconnect();
}

bool 
Session::listening(int port)
{
	//listen socket
	auto listenSocket = NetworkEngine::instance().getListenSocket(port);
	socket_->setListenSocket(listenSocket);

	return socket_->listening();
}

void 
Session::dispatch()
{
	//큐에서 패킷을 가져온다
	rgs::protocol::Packet* packet = nullptr;

	while (packets_.empty() == false)
	{
		if (packets_.try_pop(packet) == false)
		{
			return;
		}

		if (packet == nullptr)
		{
			LOG(ERROR) << "Session-" << id_ << " Session::dispatch() Error : " << rgs::Error::ERROR_NULL_DATA;
			continue;
		}
		
		if (isConnected() == true)
		{
			packet->dispatch(shared_from_this());
		}

		delete packet;
		packet = nullptr;
	}
}

void
Session::complete(rgs::io::ConnectionEvent connectionEvent)
{
	if (connectionEvent == rgs::io::ConnectionEvent::CONNECTED)
	{
		id_ = generateId();
		socket_->setConnected(true);
		LOG(INFO) << "Session-" << id_ << " is Connected (host : " <<
			ipAddress().host << " / port : " <<
			ipAddress().port << ")";

		SocketThread::instance().add(id_, socket_);
	}

	if (connectionEvent == rgs::io::ConnectionEvent::DISCONNECTED)
	{
		LOG(INFO) << "Session-" << id_ << " is Disconnected (host : " <<
			ipAddress().host << " / port : " <<
			ipAddress().port << ")";

		socket_->setConnected(false);

		SocketThread::instance().remove(id_);
	}

	handler_(connectionEvent, shared_from_this());
}

bool 
Session::isConnected()const
{
	return socket_->isConnected();
}

void Session::setTimeout(DWORD timeout)
{
	socket_->setTimeout(timeout);
}

rgs::io::IPAddress Session::ipAddress()const
{
	return socket_->ipAddress();
}

void Session::setDelay(bool useDelay)
{
	socket_->setDelay(useDelay);
}

void Session::setDelay(bool useDelay, DWORD delay)
{
	socket_->setDelay(useDelay, delay);
}

CreateSession::CreateSession(std::shared_ptr<rgs::protocol::Protocol> protocol) : 
	protocol_(protocol) {}

std::shared_ptr<Session> CreateSession::operator()()
{
	std::shared_ptr<Session> session = std::make_shared<Session>();

	session->initialize(protocol_);

	return session;
}