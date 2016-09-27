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

			/**
			@class Packet

			@brief google::protobuf 패킷을 정의하는 클래스

			(1) google::protobuf 패킷을 정의합니다.

			(2) 패킷 핸들러 디스패치(dispatch)
			-패킷 핸들러를 소유하고, dispatch()가 호출되면 패킷을 전달해야 합니다.

			(3) protocol::Deserialization을 이용해 역직렬화
			-google::protobuf를 이용해 역직렬화합니다.

			@tparam T google::protobuf::Message를 상속받는 클래스 타입

			@author 조민우
			@since 1.0.0
			*/
			template<class T>
			class Packet : public rgs::protocol::Packet
			{
			public:
				/**
				@brief 디폴트 소멸자

				@since 1.0.0
				*/
				~Packet() = default;

			public:
				/**
				@brief 패킷 핸들러 디스패치(dispatch)

				google::protobuf 패킷 객체를 패킷 핸들러를 호출하여 전달합니다.

				@param[in] session 패킷을 보낸 Session

				@return 패킷 핸들러 호출하면 true, 실패하면 false

				@since 1.0.0
				*/
				virtual bool dispatch(std::shared_ptr<rgs::io::Session> session) override
				{
					assert(handler_);

					(*handler_)(session, &packet_);

					return true;
				}

				/**
				@brief RowData에 있는 데이터를 역직렬화하여 패킷으로 저장합니다.

				protocol::Deserialization의 구현 객체를 이용해 역직렬화를 처리하게 합니다.
				그 결과로 패킷을 받아 저장합니다.

				@param[in] rowData binary stream형태의 데이터를 가진 RowData 객체

				@return 역직렬화를 성공하면 true, 실패하면 false

				@since 1.0.0
				*/
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
				/**
				@brief 패킷 핸들러를 지정합니다.

				@param[in] handler dispatch() 호출시 패킷을 전달받을 핸들러

				@since 1.0.0
				*/
				void setHandler(Handler<T>* handler)
				{
					handler_ = handler;
				}

			private:
				Handler<T>* handler_ = nullptr;
				T packet_;
			};

			/**
			@class CreatePacket

			@brief protobuf::Packet을 생성하고 초기화하는 팩토리 클래스

			@tparam T google::protobuf::Message를 상속받는 클래스 타입

			@author 조민우
			@since 1.0.0
			*/
			template<class T>
			class CreatePacket : public rgs::protocol::CreatePacket
			{
			public:
				/**
				@brief protobuf::Packet을 생성한다

				@return 새로운 Packet 객체

				@since 1.0.0
				*/
				virtual rgs::protocol::Packet* operator()()const
				{
					auto packet = new Packet<T>();

					packet->setHandler(&handler_);

					return packet;
				}

				/**
				@brief 초기화를 한다.

				@tparam F 패킷 핸들러 type

				@param[in] handler 패킷에 지정할 패킷 핸들러

				@since 1.0.0
				*/
				template<class F>
				void initialize(F&& handler)
				{
					handler_.setCallback(handler);
				}


			private:
				mutable Handler<T> handler_;
			};

			/**
			@class Serialization

			@brief google::protobuf 직렬화 클래스

			google::protobuf를 이용해 직렬화한다.

			@author 조민우
			@since 1.0.0
			*/
			class Serialization : public rgs::protocol::Serialization
			{
			public:
				/**
				@brief 디폴트 생성자

				@param[in] packetId 패킷 Id

				@param[in] message google::protobuf::Message를 상속받는 패킷

				@since 1.0.0
				*/
				explicit Serialization(int packetId, const google::protobuf::Message& message);

				/**
				@brief 디폴트 소멸자

				@since 1.0.0
				*/
				~Serialization();

			public:
				/**
				@brief 직렬화를 한다.

				@return 직렬화한 binary stream형태의 데이터를 가진 RowData 객체

				@since 1.0.0
				*/
				virtual rgs::protocol::RowData operator()();

			private:
				int packetId_ = 0;
				const google::protobuf::Message& message_;

				rgs::protocol::RowData rowData_;
			};

			/**
			@class Deserialization

			@brief google::protobuf 역직렬화 클래스

			@author 조민우
			@since 1.0.0
			*/
			class Deserialization : public rgs::protocol::Deserialization
			{
			public:
				/**
				@brief 디폴트 생성자

				@param[in] packet 역직렬화한 결과를 받을 패킷

				@since 1.0.0
				*/
				explicit Deserialization(google::protobuf::Message& packet);

			public:
				/**
				@brief 역직렬화를 한다.

				@param[in] rowData 역직렬화를 할 binary stream 형태의 데이터를 담은 RowData 객체

				@return 역직렬화를 성공하면 true, 실패하면 false

				@since 1.0.0
				*/
				virtual bool operator()(const rgs::protocol::RowData& rowData);

			private:
				google::protobuf::Message& packet_;
			};

			/**
			@class Protocol

			@brief 패킷 헤더(Packet Id, Packet Size)를 읽어 응용 레벨 프로토콜에 따라 google::protobuf 패킷 처리를 하는 클래스

			@author 조민우
			@since 1.0.0
			*/
			class Protocol : public rgs::protocol::Protocol
			{
			public:
				/**
				@brief 응용 레벨 프로토콜에 따라 google::protobuf 패킷 처리를 합니다.

				@param[in] rowData binary stream 형태의 데이터를 가진 RowData 객체
				@param[out] packet 패킷 처리의 결과를 받을 Packet 포인터. 패킷 처리가 실패하면 null이 설정된다.
				@param[out] readBytes 패킷의 바이트 크기

				@return 패킷 처리가 성공하면 0, 실패하면 에러 코드

				@since 1.0.0
				*/
				virtual int readPacket(const rgs::protocol::RowData& rowData, rgs::protocol::Packet** packet, unsigned int& readBytes)const;

				/**
				@brief 패킷과 패킷 Id, 패킷 핸들러를 등록합니다.

				지정한 패킷 Id 수신하면 지정한 템플릿 파라미터 T로 직렬화하게 됩니다.
				또한 직렬화 후 지정한 패킷 핸들러도 호출됩니다.

				@tparam T 패킷 type
				@tparam F 패킷 핸들러 type

				@param[in] packetId 패킷 Id
				@param[in] handler 패킷 핸들러

				@since 1.0.0
				*/
				template<typename T, class F>
				void registerPacket(unsigned int packetId, F&& handler);
			
			private:
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