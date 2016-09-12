//#include "GateHandler.h"
//#include "Common.h"
//
//#include "helper/Logger.h"
//#include "io/Session.h"
//
//#include "protocol/PacketType.h"
//#include "protocol/Auth.pb.h"
//#include "protocol/Protocol.h"
//#include "protocol/JsonProtocol.h"
//
//#include "App.h"
//
//std::shared_ptr<rgs::protocol::Protocol> GateHandler::createProtocol()
//{
//	auto protocol = std::make_shared < rgs::protocol::json::Protocol >();
//
//	protocol->registerHandler("login", [](std::shared_ptr<rgs::io::Session> session, nlohmann::json* login) -> void {
//
//		rgs::protocol::json::Serialization serialization("login", *login);
//		session->write(serialization());
//	});
//
//	return protocol;
//}

#include "GateHandler.h"
#include "Common.h"

#include "io/Session.h"

#include "protocol/PacketType.h"
#include "protocol/Auth.pb.h"

#include "protocol/Protocol.h"
#include "protocol/Protobuf.h"
#include "protocol/JsonProtocol.h"

#include "glog/logging.h"

#include "App.h"

std::shared_ptr<rgs::protocol::Protocol> GateHandler::createProtocol()
{
	auto protocol = std::make_shared < rgs::protocol::protobuf::Protocol >();

	protocol->registerPacket<Auth::Login>(frontend::PacketType::PacketType_Login, [](std::shared_ptr<rgs::io::Session> session, Auth::Login* login) -> void {

		auto pairSession = session->pairSession;
		if (pairSession != nullptr)
		{
			rgs::protocol::protobuf::Serialization serialization(1, *login);
			pairSession->write(serialization());
			
		}
		else
		{
			rgs::protocol::protobuf::Serialization serialization(1, *login);
			session->write(serialization());
		}
	});

	return protocol;
}