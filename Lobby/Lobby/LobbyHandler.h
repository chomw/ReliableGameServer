#pragma once
#include <memory>

namespace rgs
{
	namespace protocol
	{
		class Protocol;
	}
}

class LobbyHandler
{
public:
	static std::shared_ptr<rgs::protocol::Protocol> createProtocol();
};