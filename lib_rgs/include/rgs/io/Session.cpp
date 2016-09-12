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
socket_(std::make_shared<Socket>(*this)), 
connection_(nullptr) {}

Session::Session(rgs::io::Connection* connection) :
socket_(std::make_shared<Socket>(*this)),
connection_(connection) {}

Session::~Session()
{
	SocketMonitor::instance().remove(id_);
}

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
Session::initialize(std::shared_ptr<rgs::protocol::Protocol> protocol, int port)
{	
	id_ = generateId();

	if (socket_->createRowSocket() == false)
	{
		return false;
	}

	protocol_ = protocol;

	//proactor 초기화
	NetworkEngine::instance().registerSocket(socket_);

	//listen socket
	if (port > 0)
	{
		socket_->setListenSocket(NetworkEngine::instance().getListenSocket(port));
	}

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

		if (NetworkEngine::instance().getCore() == rgs::Core::MULTI)
		{
			error = protocol_->dispatchPacket(packetRowData, shared_from_this(), readBytes);
		}
		else
		{
			error = protocol_->readPacket(packetRowData, &packet, readBytes);
		}

		if (error)
		{
			LOG(ERROR) << "Session-" << id_ << " Session::read() Error : " << error;
			return error;
		}

		if (readBytes == 0)
		{
			break;
		}

		packetRowData.data += readBytes;
		packetRowData.size -= readBytes;

		if (NetworkEngine::instance().getCore() == rgs::Core::SINGLE)
		{
			if(packet == nullptr)
			{
				error = rgs::Error::ERROR_NULL_DATA;
				LOG(ERROR) << "Session-" << id_ << " Session::read() Error : " << error;
				return error;
			}
			else
			{
				packets_.push(packet);
			}
		}
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
Session::connect(const std::string& address, int port, const rgs::protocol::RowData* rowData)
{
	bool result = false;

	if (rowData != nullptr)
	{
		result = socket_->connect(address, port, rowData->data, rowData->size);
	}
	else
	{
		result = socket_->connect(address, port);
	}
	
	return result;
}

bool
Session::disconnect()
{
	return socket_->disconnect();
}

bool 
Session::listening()
{
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
Session::complete(rgs::io::IoEvent ioEvent)
{
	if (ioEvent == rgs::io::IoEvent::IO_EVENT_CONNECTED)
	{
		socket_->setConnected(true);
		LOG(INFO) << "Session-" << id_ << " is Connected (host : " <<
			ipAddress_.host << " / port : " <<
			ipAddress_.port << ")";

		SocketMonitor::instance().add(id_, socket_);
	}

	if (ioEvent == rgs::io::IoEvent::IO_EVENT_DISCONNECTED)
	{
		LOG(INFO) << "Session-" << id_ << " is Disconnected (host : " <<
			ipAddress_.host << " / port : " <<
			ipAddress_.port << ")";

		socket_->setConnected(false);

		SocketMonitor::instance().remove(id_);
	}

	if (connection_)
	{
		connection_->complete(ioEvent, shared_from_this());
	}
}

bool 
Session::isConnected()const
{
	return socket_->isConnected();
}


void Session::useKeepAlive(bool useKeepAlive)
{
	socket_->useKeepAlive(useKeepAlive);
}

void Session::setKeepAliveInterval(DWORD interval)
{
	socket_->setKeepAliveInterval(interval);
}

void Session::ipAddress(const rgs::io::IPAddress& ipAddress)
{
	ipAddress_ = ipAddress;
}

rgs::io::IPAddress Session::ipAddress()const
{
	return ipAddress_;
}

void Session::setDelay(bool useDelay)
{
	socket_->setDelay(useDelay);
}

void Session::setDelay(bool useDelay, DWORD delay)
{
	socket_->setDelay(useDelay, delay);
}
