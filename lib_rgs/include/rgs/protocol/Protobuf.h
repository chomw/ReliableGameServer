#pragma once

#pragma comment(lib, "libprotobuf.lib")

#include <memory>
#include <utility>

#include "protocol.h"

#include "../thread/Synchronous.h"
#include "../callback/Callback.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/coded_stream.h>

namespace rgs
{
	namespace protocol
	{
		/**
		@namespace protobuf
		@brief Google Protocol Buffer 기반 프로토콜 네임스페이스
		@since 1.0.0
		*/
		namespace protobuf
		{
			template<class T>
			using Handler = rgs::cb::Callback<std::shared_ptr<rgs::io::Session>, T*>;

			template<class T>
			class Packet : public rgs::protocol::Packet
			{
			public:
				~Packet() = default;

			public:
				virtual bool dispatch(std::shared_ptr<rgs::io::Session> session) override
				{
					assert(handler_);

					(*handler_)(session, &packet_);

					return true;
				}

				virtual bool deserialize(const rgs::protocol::RowData& rowData) override
				{
					Deserialization deserialization(packet_);
					if (deserialization(rowData) == false)
					{
						return false;
					}

					return true;
				}

			public:
				void setHandler(Handler<T>* handler)
				{
					handler_ = handler;
				}

			private:
				Handler<T>* handler_ = nullptr;
				T packet_;
			};

			template<class T>
			class CreatePacket : public rgs::protocol::CreatePacket
			{
			public:
				virtual rgs::protocol::Packet* operator()()const
				{
					auto packet = new Packet<T>();

					packet->setHandler(&handler_);

					return packet;
				}

				template<class F>
				void initialize(F&& handler)
				{
					handler_.setCallback(handler);
				}


			private:
				mutable Handler<T> handler_;
			};

			class Serialization : public rgs::protocol::Serialization
			{
			public:
				explicit Serialization(int packetId, const google::protobuf::Message& message);
				~Serialization();

			public:
				virtual rgs::protocol::RowData operator()();

			private:
				int packetId_ = 0;
				const google::protobuf::Message& message_;

				rgs::protocol::RowData rowData_;
			};

			class Deserialization : public rgs::protocol::Deserialization
			{
			public:
				explicit Deserialization(google::protobuf::Message& packet);

			public:
				virtual bool operator()(const rgs::protocol::RowData& rowData);

			private:
				google::protobuf::Message& packet_;
			};


			class Protocol : public rgs::protocol::Protocol
			{
			public:
				virtual int readPacket(const rgs::protocol::RowData& rowData, rgs::protocol::Packet** packet, unsigned int& readBytes)const;

				template<typename T, class F>
				void registerPacket(unsigned int packetId, F&&);
			
			protected:
				unsigned int createPacketSize_ = 0;
				std::vector<std::shared_ptr<rgs::protocol::Packet>> packets_;
				std::vector<std::shared_ptr<rgs::protocol::CreatePacket>> createPackets_;

				mutable rgs::thread::sync::SimpleLock lock_;
			};

			template<typename T, class F>
			void Protocol::registerPacket(unsigned int packetId, F&& func)
			{
				rgs::thread::sync::scope scope(&lock_);

				if (packetId >= createPackets_.size())
				{
					createPackets_.resize(packetId + 1);
					createPacketSize_ = packetId + 1;

					packets_.resize(packetId + 1);
				}

				auto createPacket = std::make_shared<CreatePacket<T>>();
				createPacket->initialize(func);

				createPackets_[packetId] = createPacket;
				packets_[packetId] = std::shared_ptr<rgs::protocol::Packet>((*createPacket.get())());
			}
		}
	}
}