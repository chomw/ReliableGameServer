#pragma once
#include "Protocol.h"
#include "json.hpp"

#include "../thread/Synchronous.h"
#include "../callback/Callback.h"

namespace rgs
{
	namespace protocol
	{
		/**
		@namespace json
		@brief json 기반 프로토콜 네임스페이스
		@since 1.0.0
		*/
		namespace json
		{
			enum
			{
				MAX_PACKET_NAME = 64
			};

			using Handler = rgs::cb::Callback<std::shared_ptr<rgs::io::Session>, nlohmann::json*>;

			class Packet : public rgs::protocol::Packet
			{
			public:
				~Packet();

			public:
				virtual bool dispatch(std::shared_ptr<rgs::io::Session> session) override;

				virtual bool deserialize(const rgs::protocol::RowData& rowData) override;
				
			public:
				void setHandler(Handler* handler);

			private:
				rgs::protocol::RowData rowData_;
				Handler* handler_ = nullptr;
			};

			class Serialization : public rgs::protocol::Serialization
			{
			public:
				explicit Serialization(const std::string& packetName, const nlohmann::json& packet);
				~Serialization();

			public:
				virtual rgs::protocol::RowData operator()();

			private:
				const std::string& packetName_;
				const nlohmann::json& packet_;

				RowData rowData_;
			};

			class Protocol : public rgs::protocol::Protocol
			{
			public:
				virtual int readPacket(const rgs::protocol::RowData& rowData, rgs::protocol::Packet** packet, unsigned int& readBytes)const;

			public:
				template<class F>
				void registerHandler(const std::string& packetName, F&& func)
				{
					rgs::thread::sync::scope scope(&lock_);
					handlers_[packetName] = std::make_shared<Handler>(func);
				}

			private:
				mutable rgs::thread::sync::SimpleLock lock_;
				std::map<std::string, std::shared_ptr<Handler>> handlers_;
			};
		}
	}
}