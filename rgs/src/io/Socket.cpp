#include "stdafx.h"
#include "../Common.h"

#include <ppl.h>
#include <concurrent_queue.h>

#include "Socket.h"
#include "Session.h"

#include "../helper/Logger.h"
#include "../protocol/Protocol.h"
#include "../thread/SingleThread.h"

#include "proactor/Proactor.h"
#include "proactor/Acceptor.h"
#include "proactor/Disconnector.h"
#include "proactor/Receiver.h"
#include "proactor/Sender.h"
#include "proactor/Connector.h"

using namespace rgs::io;

LPFN_CONNECTEX Socket::ConnectEx;

Socket::Socket(Session& session) :
	rowSocket_(INVALID_SOCKET),
	proactor_(nullptr),
	listenSocket_(nullptr),
	session_(session),
	previousSendTime_(0),
	delayTime_(0),
	lastCheckTime_(0),
	delay_(Socket::DEFAULT_DELAY),
	writeCount_(0),
	isConnect_(false),
	isRegisterToIocp_(false),
	isCanceled_(false),
	useKeepAlive_(false),
	useDelay_(true),
	keepAliveInterval_(30000),
	keepAliveTimestamp_(0),
	currentState_(State::DISCONNECTED),
	operatingState_(State::DISCONNECTED) {}

Socket::~Socket()
{
	if (isValidRowSocket() == true)
	{
		::closesocket(rowSocket_);
	}
}

bool Socket::createRowSocket()
{
	setConnected(false);
	setRegisterToIocp(false);

	currentState_ = State::DISCONNECTED;

	if (isValidRowSocket() == true)
	{
		::closesocket(rowSocket_);
		rowSocket_ = INVALID_SOCKET;
	}

	rowSocket_ = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (isValidRowSocket() == false)
	{
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::WSASocket() Error : " << WSAGetLastError();
		return false;
	}

	return true;
}

bool Socket::write(BYTE* data, unsigned int size)
{
	assert(data);

	if (isConnected() == false)
	{
		return false;
	}

	rgs::thread::sync::scope scope(&writeLock_);
	
	if (useDelay_ == true)
	{
		if (sendSize_ + size >= rgs::SocketConfig::BUFFER_SIZE)
		{
			if (send(sendBuffer_, sendSize_) == false)
			{
				return false;
			}
			sendSize_ = 0;
		}

		::memcpy(sendBuffer_ + sendSize_, data, size);

		sendSize_ += size;
		writeCount_++;

		DWORD currentTime = ::timeGetTime();
		if (lastCheckTime_ <= currentTime)
		{
			//지연시간을 계산한다
			delayTime_ = (writeCount_ / IMMEDIATELY_SEND);

			writeCount_ = 0;
			lastCheckTime_ = currentTime + delay_;
		}

		if (previousSendTime_ + delayTime_ <= currentTime)
		{
			if (send(sendBuffer_, sendSize_) == false)
			{
				return false;
			}

			sendSize_ = 0;
			previousSendTime_ = ::timeGetTime();
		}
	}
	else
	{
		//즉시 전송
		if (sendSize_ > 0)
		{
			send(sendBuffer_, sendSize_);
			sendSize_ = 0;
		}

		send(data, size);
	}

	return true;
}

bool Socket::startReceive()
{
	if (isConnected() == false)
	{
		return false;
	}

	if (isValidRowSocket() == false)
	{
		return false;
	}

	WSABUF wsaRecvBuf;
	wsaRecvBuf.buf = (char*)receiveBuffer_ + receiveSize_;
	wsaRecvBuf.len = rgs::SocketConfig::BUFFER_SIZE - receiveSize_;

	DWORD flags = 0;
	int ret = ::WSARecv(rowSocket_, &wsaRecvBuf, 1, NULL, &flags, static_cast<OVERLAPPED*>(&acts_[ACT_TYPE::ACT_RECV]), NULL);
	if (ret != SOCKET_ERROR)
	{
		return true;
	}

	int error = WSAGetLastError();

	if (error == WSAEWOULDBLOCK)
	{
		return true;
	}

	if (error == ERROR_IO_PENDING)
	{
		return true;
	}

	//row socket이 연결이 끊기거나 close된 이후의 receive로 인한 에러는 무시한다.
	if (error != WSAECONNRESET && error != WSAENOTCONN && error != WSAECONNABORTED && error != WSAESHUTDOWN)
	{
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::WSARecv() Error : " << error;
	}

	disconnect();

	return false;
}

bool Socket::receive(unsigned int receiveBytes)
{
	if (isConnected() == false)
	{
		return false;
	}

	if (isValidRowSocket() == false)
	{
		return false;
	}

	setKeepAliveTimestamp(::timeGetTime());

	receiveSize_ += receiveBytes;

	//패킷을 처리한다
	rgs::protocol::RowData rowData;

	rowData.data = receiveBuffer_;
	rowData.size = receiveSize_;

	unsigned int readBytes = 0;

	unsigned int error = session_.read(rowData, readBytes);

	if (error)
	{
		return false;
	}

	//남은 패킷을 버퍼의 맨앞으로 붙인다
	receiveSize_ -= readBytes;
	if (receiveSize_ > 0)
	{
		::memcpy(receiveBuffer_, receiveBuffer_ + readBytes, receiveSize_);
	}

	return true;
}

bool Socket::bind()
{
	if (isValidRowSocket() == false)
	{
		return false;
	}

	BOOL valid = 1;

	setsockopt(rowSocket_,
		SOL_SOCKET,
		SO_REUSEADDR,
		(const char *)&valid,
		sizeof(valid));


	SOCKADDR_IN addrInfo;
	::ZeroMemory(&addrInfo, sizeof(SOCKADDR_IN));
	addrInfo.sin_family = AF_INET;
	addrInfo.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);

	int ret = ::bind(rowSocket_, (struct sockaddr*)&addrInfo, sizeof(addrInfo));
	if (ret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::bind() Error : " << error;
		return false;
	}

	return true;
}

bool Socket::connect(const std::string& address, USHORT port, BYTE* data, unsigned int size)
{
	if (isValidRowSocket() == false)
	{
		return false;
	}
	
	currentState_ = State::CONNECTING;
	operatingState_ = State::CONNECTING;

	//완료 통지를 받기 위해 proactor에 등록한다.
	if (getRegisterToIocp() == false)
	{
		proactor_->registerToIocp(rowSocket_);
		setRegisterToIocp(true);
	}

	struct linger solinger = { 1, 0 };
	setsockopt(rowSocket_, SOL_SOCKET, SO_LINGER, (char FAR*)&solinger, sizeof(struct linger));

	SOCKADDR_IN addrInfo;
	addrInfo.sin_family = AF_INET;
	addrInfo.sin_port = htons(port);
	addrInfo.sin_addr.S_un.S_addr = inet_addr(address.c_str());

	DWORD sendBytes = 0;
	
	BOOL ret = ConnectEx(rowSocket_, (SOCKADDR*)&addrInfo, sizeof(addrInfo), data, size, &sendBytes, static_cast<OVERLAPPED*>(&acts_[ACT_TYPE::ACT_CONNECT]));

	if (ret == FALSE)
	{
		int error = WSAGetLastError();
		if (error != ERROR_IO_PENDING)
		{
			RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ConnectEx() Error : " << error;
			return false;
		}
	}

	return true;
}

bool Socket::disconnect()
{
	rgs::thread::sync::scope scope(&connectionLock_);
	
	if (isValidRowSocket() == false)
	{
		return false;
	}

	if (isCanceled_ == true)
	{
		return false;
	}

	switch (currentState_)
	{
	case State::DISCONNECTED:
	case State::DISCONNECTING:
		return false;
	case State::CONNECTING:
	case State::LISTENING:
		isCanceled_ = true;
	default:
		break;
	}

	//operatingState_에 따라 다르게 연결을 종료
	if (operatingState_ == State::CONNECTING)
	{
		currentState_ = State::DISCONNECTED;

		if (isValidRowSocket() == true)
		{
			::closesocket(rowSocket_);
			rowSocket_ = INVALID_SOCKET;
		}

		session_.complete(rgs::io::IoEvent::IO_EVENT_DISCONNECTED);
	}
	else
	{
		currentState_ = State::DISCONNECTING;

		BOOL ret = ::TransmitFile(rowSocket_, NULL, 0, 0, static_cast<OVERLAPPED*>(&acts_[ACT_TYPE::ACT_DISCONNECT]), NULL, TF_DISCONNECT | TF_REUSE_SOCKET);

		if (ret == TRUE)
		{
			return true;
		}
		
		int error = ::WSAGetLastError();
		if (error != ERROR_IO_PENDING)
		{
			if (error != WSAENOTCONN)
			{
				RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::TramsmitFile() Error : " << error;
			}

			return false;
		}
	}

	return true;
}

bool Socket::listening()
{
	if (listenSocket_ == nullptr)
	{
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " listening Error : " << rgs::Error::ERROR_NULL_DATA;
		return false;
	}

	if (listenSocket_->listen(this, (char*)receiveBuffer_, 0, &acts_[Socket::ACT_ACCEPT]) == false)
	{
		return false;
	}

	currentState_ = State::LISTENING;
	operatingState_ = State::LISTENING;

	return true;
}

bool Socket::readAddress()
{
	if (isValidRowSocket() == false)
	{
		return false;
	}

	SOCKADDR* l_addr = nullptr; SOCKADDR* r_addr = nullptr; int l_size = 0, r_size = 0;
	GetAcceptExSockaddrs(receiveBuffer_,
		0,
		sizeof(SOCKADDR_STORAGE) + 16,
		sizeof(SOCKADDR_STORAGE) + 16,
		&l_addr,
		&l_size,
		&r_addr,
		&r_size
	);

	SOCKADDR_IN address = { 0 };
	int addressSize = sizeof(address);

	int error = ::getpeername(rowSocket_, (struct sockaddr*)&address, &addressSize);

	if (error)
	{
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::getpeername() Error : " << ::WSAGetLastError();
		return false;
	}

	session_.ipAddress(rgs::io::IPAddress(
		::inet_ntoa(address.sin_addr),
		::ntohs(address.sin_port)
	));

	return true;
}

void Socket::complete(rgs::io::IoEvent ioEvent)
{
	switch (ioEvent)
	{
	case rgs::io::IoEvent::IO_EVENT_CONNECTED:
	{
		initialize();
		
		rgs::thread::sync::scope scope(&connectionLock_);
		
		//연결되고 나서 기존 socket 설정을 UPDATE시켜주어야 한다.
		if (operatingState_ == State::LISTENING)
		{
			::SOCKET listenRowSocket = listenSocket_->getRowSocket();
			::setsockopt(rowSocket_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&listenRowSocket, (int)sizeof(SOCKET));
		}

		if (operatingState_ == State::CONNECTING)
		{
			::setsockopt(rowSocket_, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
		}

		readAddress();
		
		if (getRegisterToIocp() == false)
		{
			proactor_->registerToIocp(reinterpret_cast<::HANDLE>(getRowSocket()));
			setRegisterToIocp(true);
		}

		setKeepAliveTimestamp(::timeGetTime());
		
		session_.complete(ioEvent);
		
		currentState_ = State::CONNECTED;

		if (isCanceled_ == true)
		{
			isCanceled_ = false;
			disconnect();
		}

	}
	break;
	case rgs::io::IoEvent::IO_EVENT_DISCONNECTED:
	{
		currentState_ = State::DISCONNECTED;
		session_.complete(ioEvent);
	}
	break;
	}
}

bool Socket::completeListening()
{
	if (currentState_ != State::LISTENING)
	{
		return false;
	}

	if (isValidRowSocket() == false)
	{
		return false;
	}

	INT seconds;
	INT bytes = sizeof(seconds);
	int error = 0;

	error = getsockopt(rowSocket_, SOL_SOCKET, SO_CONNECT_TIME,
		(char *)&seconds, (PINT)&bytes);

	if (error != NO_ERROR) {
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::getsockopt(SO_CONNECT_TIME)() Error : " << WSAGetLastError();
		return false;
	}

	if (seconds > 5)
	{
		createRowSocket();
		listening();

		return true;
	}

	return false;
}

void Socket::sendDelayed()
{
	if (isConnected() == false)
	{
		return;
	}

	rgs::thread::sync::scope scope(&writeLock_);

	if (sendSize_ <= 0)
	{
		return;
	}

	if (previousSendTime_ + Socket::DEFAULT_DELAY <= ::timeGetTime())
	{
		send(sendBuffer_, sendSize_);
		sendSize_ = 0;
	}
}

void Socket::setConnected(bool connected)
{
	isConnect_ = connected;
}

void Socket::setRegisterToIocp(bool registerToIocp)
{
	isRegisterToIocp_ = registerToIocp;
}

void Socket::setProactor(rgs::io::proactor::Proactor* proactor, rgs::io::proactor::Acceptor* acceptor, rgs::io::proactor::Disconnector* disconnector, rgs::io::proactor::Sender* sender, rgs::io::proactor::Receiver* receiver, rgs::io::proactor::Connector* connector)
{
	proactor_ = proactor;

	acts_[ACT_ACCEPT].initialize(acceptor, this);
	acts_[ACT_RECV].initialize(receiver, this);
	acts_[ACT_SEND].initialize(sender, this);
	acts_[ACT_DISCONNECT].initialize(disconnector, this);
	acts_[ACT_CONNECT].initialize(connector, this);
}

void Socket::setListenSocket(std::shared_ptr<ListenSocket> listenSocket)
{
	listenSocket_ = listenSocket;
}

::SOCKET Socket::getRowSocket()const
{
	return rowSocket_;
}

bool Socket::getRegisterToIocp()const
{
	return isRegisterToIocp_;
}

bool Socket::isConnected()const
{
	return isConnect_;
}

bool Socket::isValidRowSocket()const
{
	return rowSocket_ != INVALID_SOCKET;
}

void Socket::initialize()
{
	{
		receiveSize_ = 0;
		::ZeroMemory(receiveBuffer_, rgs::SocketConfig::BUFFER_SIZE);
	}
	{
		sendSize_ = 0;
		::ZeroMemory(sendBuffer_, rgs::SocketConfig::BUFFER_SIZE);
	}
}

bool Socket::send(BYTE* data, unsigned int size)
{
	if (isConnected() == false)
	{
		return false;
	}

	if (isValidRowSocket() == false)
	{
		return false;
	}

	WSABUF wsaSendBuf;
	wsaSendBuf.buf = reinterpret_cast<char*>(data);
	wsaSendBuf.len = size;

	int ret = ::WSASend(rowSocket_, &wsaSendBuf, 1, NULL, 0, static_cast<OVERLAPPED*>(&(acts_[ACT_TYPE::ACT_SEND])), NULL);

	setKeepAliveTimestamp(::timeGetTime());

	if (ret != SOCKET_ERROR)
	{
		return true;
	}

	int error = WSAGetLastError();

	if (error == ERROR_IO_PENDING)
	{
		return true;
	}

	//row socket이 연결이 끊기거나 close된 이후의 send로 인한 에러는 무시한다.
	if (error != WSAECONNRESET && error != WSAENOTCONN && error != WSAECONNABORTED)
	{
		RGS_LOG(severity_level::error) << "Socket-" << session_.id() << " / RowSocket : " << rowSocket_ << " / ::WSASend() Error : " << error;
	}

	disconnect();

	return false;
}

void Socket::useKeepAlive(bool useKeepAlive)
{
	useKeepAlive_ = useKeepAlive;
}

bool Socket::isExpired()
{
	if (isConnected() == false)
	{
		return false;
	}

	if (useKeepAlive_ == false)
	{
		return false;
	}

	DWORD elapsedTime = ::timeGetTime() - keepAliveTimestamp_;
	return keepAliveInterval_ <= elapsedTime;
}

void Socket::setKeepAliveInterval(DWORD interval)
{
	keepAliveInterval_ = interval;
}

void Socket::setKeepAliveTimestamp(DWORD time)
{
	keepAliveTimestamp_ = time;
}

void Socket::setDelay(bool useDelay)
{
	useDelay_ = useDelay;
}

void Socket::setDelay(bool useDelay, DWORD delay)
{
	delay_ = delay;
	setDelay(useDelay);
}

void ListenSocket::initialize(rgs::io::proactor::Proactor* proactor, unsigned int port)
{
	rowSocket_ = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN Addr_;
	Addr_.sin_family = AF_INET;
	Addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	Addr_.sin_port = htons(port);

	proactor->registerToIocp(reinterpret_cast<HANDLE>(rowSocket_));

	int ret = ::bind(rowSocket_, (SOCKADDR*)&Addr_, sizeof(Addr_));
	if (ret == SOCKET_ERROR)
	{
		int error = ::WSAGetLastError();
		RGS_LOG(severity_level::error) << "Listen Socket / RowSocket : " << rowSocket_ << " / ::bind() Error : " << error;
	}

	ret = ::listen(rowSocket_, 0);
	if (ret == SOCKET_ERROR)
	{
		int error = ::WSAGetLastError();
		RGS_LOG(severity_level::error) << "Listen Socket / RowSocket : " << rowSocket_ << " / ::listen() Error : " << error;
	}
}

bool ListenSocket::listen(Socket* socket, char* buffer, unsigned int bufferSize, rgs::io::proactor::Act* act)
{
	if (socket->isConnected() == true)
	{
		return false;
	}

	DWORD transferredBytes = 0;
	BOOL ret = ::AcceptEx(
		rowSocket_,
		socket->getRowSocket(),
		buffer,
		bufferSize,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&transferredBytes,
		static_cast<OVERLAPPED*>(act)
		);

	if (ret == FALSE)
	{
		int error = ::WSAGetLastError();

		if (error != ERROR_IO_PENDING)
		{
			RGS_LOG(severity_level::error) << "RowSocket : " << socket->getRowSocket() << " / ::AcceptEx() Error : " << error;
			return false;
		}
	}

	return true;
}

::SOCKET ListenSocket::getRowSocket()const
{
	return rowSocket_;
}


class SocketMonitor::Implementation
{
public:
	struct AddedSocket
	{
		AddedSocket() = default;
		AddedSocket(int id, std::shared_ptr<Socket> socket) :
			id(id),
			socket(socket) {}

		int id;
		std::shared_ptr<Socket> socket;
	};
public:
	void update()
	{
		//모니터링할 소켓을 추가한다
		AddedSocket addedSocket;
		while (queue_.empty() == false)
		{
			if (queue_.try_pop(addedSocket) == false)
			{
				break;
			}

			if (addedSocket.socket == nullptr)
			{
				sockets_.erase(addedSocket.id);
			}
			else
			{
				sockets_[addedSocket.id] = addedSocket.socket;
			}
			
		}

		if (sockets_.size() <= 0)
		{
			return;
		}

		for (auto iterator : sockets_)
		{
			auto socket = iterator.second;

			if (socket == nullptr)
			{
				continue;
			}

			if (socket->isConnected() == true)
			{
				//지연된 데이터를 전송한다
				socket->sendDelayed();

				//keep alive
				if (socket->isExpired() == true)
				{
					socket->disconnect();
				}
			}
		}
	};

public:
	std::shared_ptr<rgs::thread::Thread<>> thread_;
	concurrency::concurrent_queue<AddedSocket> queue_;
	std::map<int, std::shared_ptr<Socket>> sockets_;
};

std::unique_ptr<SocketMonitor> SocketMonitor::instance_ = nullptr;

SocketMonitor::SocketMonitor()
{
	impl = std::make_unique<SocketMonitor::Implementation>();

	impl->thread_ = std::make_shared<rgs::thread::Thread<>>();
	
	impl->thread_->setCallback(std::bind(&Implementation::update, impl.get()));
	impl->thread_->setUpdateInterval(rgs::Delay::SECOND_MILLI * Socket::DEFAULT_DELAY);
	impl->thread_->start();
}

SocketMonitor::~SocketMonitor()
{
	release();
}

void SocketMonitor::release()
{
	if (impl->thread_ != nullptr)
	{
		impl->thread_->stop();
		impl->thread_ = nullptr;
	}
	
	impl->sockets_.clear();
}

void SocketMonitor::add(int key, std::shared_ptr<Socket> socket)
{
	impl->queue_.push(Implementation::AddedSocket(key, socket));
}

void SocketMonitor::remove(int key)
{
	impl->queue_.push(Implementation::AddedSocket(key, nullptr));
}

SocketMonitor& SocketMonitor::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<SocketMonitor>();
	}

	return *instance_.get();
}