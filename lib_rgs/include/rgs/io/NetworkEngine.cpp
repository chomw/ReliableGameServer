#include "stdafx.h"
#include "../Common.h"
#include "NetworkEngine.h"
#include "Socket.h"
#include "Session.h"

#include "proactor/Proactor.h"
#include "proactor/Acceptor.h"
#include "proactor/Disconnector.h"
#include "proactor/Receiver.h"
#include "proactor/Sender.h"
#include "proactor/Connector.h"

using namespace rgs::io;
using namespace rgs::io::proactor;

std::unique_ptr<NetworkEngine> NetworkEngine::instance_ = nullptr;

NetworkEngine::NetworkEngine() :
proactor_(nullptr), 
acceptor_(nullptr), 
disconnector_(nullptr), 
sender_(nullptr), 
receiver_(nullptr), 
connector_(nullptr) 
{
	proactor_ = std::make_unique<Proactor>();
	acceptor_ = std::make_unique<Acceptor>();
	receiver_ = std::make_unique<Receiver>();
	disconnector_ = std::make_unique<Disconnector>();
	sender_ = std::make_unique<Sender>();
	connector_ = std::make_unique<Connector>();
}

NetworkEngine::~NetworkEngine(){}

NetworkEngine& NetworkEngine::instance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<NetworkEngine>();
	}
	return *instance_.get();
}

void NetworkEngine::start()
{
	proactor_->begin(8);
}

void NetworkEngine::stop()
{
	proactor_->end();
}

void NetworkEngine::registerSocket(std::shared_ptr<rgs::io::Socket> socket)
{
	socket->setProactor(
		proactor_.get(),
		acceptor_.get(),
		disconnector_.get(),
		sender_.get(),
		receiver_.get(),
		connector_.get()
		);
}

void NetworkEngine::setCore(rgs::Core core)
{
	core_ = core;
}

std::shared_ptr<ListenSocket> NetworkEngine::getListenSocket(int port)
{
	std::shared_ptr<ListenSocket> listenSocket = listenSockets_[port];

	if (listenSocket == nullptr)
	{
		//listen socket 생성
		listenSocket = std::make_shared<ListenSocket>();
		listenSockets_[port] = listenSocket;

		//listen socket 초기화
		listenSocket->initialize(proactor_.get(), port);
	}

	return listenSocket;
}

rgs::Core NetworkEngine::getCore()const
{
	return core_;
}