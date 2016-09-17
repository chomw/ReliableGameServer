/**
@file Session.h
@author 조민우(cmw1728@naver.com)
@since 1.0.0	
*/

#pragma once
#include <atomic>
#include "../protocol/Protocol.h"
#include "../callback/Callback.h"

#include <ppl.h>
#include <concurrent_queue.h>

namespace rgs
{
	namespace io
	{
		enum class ConnectionEvent;
		
		class Socket;
		
		/**
		@class Session
		
		@brief 응용 Level에서 IO를 수행하는 클래스.
		
		(1) Socket Wrapper :
		rgs::io::Socket을 wrapping하여 응용 Level에서 IO를 담당한다.		
	
		(2) 커넥션 이벤트 처리 :
		Socket으로부터 커넥션 이벤트(rgs::ConnectionEvent)를 받게 된다.
		내부적으로 이벤트 처리 후 등록된 이벤트 핸들러를 호출한다.

		(3) 패킷 처리 :
		Socket이 수신한 binary stream 데이터를 rgs::protocol::Protocol이 처리하도록 전달한다.
		그 결과로 얻은 rgs::protocol::Packet의 핸들러를 호출한다.
		 
		@author 조민우
		@since 1.0.0
		*/
		class Session : public std::enable_shared_from_this<Session>
		{
		public:
			/// @name Constructors and Destructor
			/// @{

			/**
			@brief 디폴트 생성자
			
			@since 1.0.0
			*/
			Session();
			
			/**
			@brief 디폴트 소멸자
			
			@since 1.0.0
			*/
			virtual ~Session() = default;

			///@}

		public:
			/// @name IO Functions
			/// @{
			/**
			@brief Socket의 RowSocket 초기화
			
			Socket::rowSocket_만 초기화한다.
			그 외에 다른 데이터는 초기화하지 않는다.
			
			@return Socket이 rowSocket_을 성공적으로 초기화하면 true, 실패하면 false
			
			@note 부득이하게 Socket::rowSocket_을 다시 생성해야 할 때만 사용한다.
			 
			@since 1.0.0
			*/
			bool initialize();
			
			/**
			@brief 모든 데이터를 초기화한다.
			
			Session의 모든 데이터를 초기화한다. 일반적으로 초기화할 때 사용한다.
			 
			@param[in] protocol 수신한 binary stream형태의 데이터를 처리할 객체			
			
			@return 성공적으로 초기화하면 true, 실패하면 false
			
			@since 1.0.0
			*/
			bool initialize(std::shared_ptr<rgs::protocol::Protocol> protocol);

			/**
			@brief 데이터를 전송한다.
			
			binary stream형태의 데이터를 전송한다.
			
			@param[in] rowData binary stream형태의 전송할 데이터
			
			@return 성공적으로 전송하면 true, 실패하면 false
			
			@since 1.0.0
			*/
			bool write(const rgs::protocol::RowData& rowData);

			/**
			@brief 데이터를 수신한다.
			
			binary stream형태의 데이터를 처리한다.
			
			@param[in] rowData 수신한 데이터
			@param[out] 처리한 데이터의 크기
			
			@return 성공적으로 처리하면 0, 실패하면 rgs::Error
			
			@since 1.0.0
			*/
			unsigned int read(
				const rgs::protocol::RowData& rowData
				, unsigned int& readBytes
			);

			/**
			@brief 소켓에 지정된 주소를 할당한다.

			@return 성공적으로 할당하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool bind();

			/**
			@brief remote host로 연결 요청을 한다.

			@param[in] address remote host ip
			@param[in] port remote host port
			@param[in] data 초기 데이터
			@param[in] size 초기 데이터 크기

			@return 성공적으로 연결 요청을 하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool connect(
				const std::string& address
				, int port
				, const BYTE* data = nullptr
				, unsigned int size = 0
			);

			/**
			@brief 연결을 종료한다.

			@return 성공적으로 연결 종료을 하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool disconnect();

			/**
			@brief 연결 대기한다.

			@param[in] port 연결 대기할 포트

			@return 성공적으로 연결 대기하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool listening(int port);

			/**
			@brief 패킷을 처리한다.

			패킷의 핸들러를 호출하여 처리한다.

			@since 1.0.0
			*/
			void dispatch();

			/**
			@brief 커넥션 이벤트를 발생시킨다
			
			Socket에 의해 커넥션 이벤트가 발생된다.
			registerHandler()로 등록된 커넥션 이벤트 핸들러를 호출한다.

			@param[in] connectionEvent 커넥션 이벤트

			@since 1.0.0
			*/
			void complete(rgs::io::ConnectionEvent connectionEvent);

			/**
			@brief 커넥션 이벤트 핸들러를 등록한다.
			
			@param[in] handler 커넥션 이벤트 발생시 호출될 핸들러

			@since 1.0.0
			*/
			template<class EventHandler>
			void registerHandler(EventHandler&& handler);

			///@}

		public:
			/// @name Data Access And Inspection
			/// @{

			/**
			@brief Session에 할당된 id를 얻는다.

			@return 할당된 id

			@since 1.0.0
			*/
			unsigned int id()const
			{
				return id_;
			}

			/** 
			@brief 연결됐는지 여부

			@return 연결되었으면 true, 연결되지 않았으면 false

			@since 1.0.0
			*/
			bool isConnected()const;
			
			/** 
			@brief 세션의 timeout될 시간을 설정한다.

			지정된 timeout 동안 IO가 없으면 연결이 종료된다.

			@param[in] timeout timeout될 시간 간격. 0을 지정하면 timeout되지 않는다.

			@since 1.0.0
			*/
			void setTimeout(DWORD timeout);

			/** 
			@brief 연결된 remote host의 주소를 얻는다.

			IPv4 ip, port를 얻는다.

			@return ip, port 주소

			@since 1.0.0
			*/
			rgs::io::IPAddress ipAddress()const;

			/** 
			@brief 전송 지연 여부를 지정한다.

			@param[in] useDelay 전송 지연 여부

			@since 1.0.0
			*/
			void setDelay(bool useDelay);

			/** 
			@brief 전송 지연 여부와 최대 지연 시간을 지정한다.

			전송 지연되더라도 Socket::DEFAULT_DELAY (mille second) 
			이내에 전송될 것을 보장한다.

			@param[in] useDelay 전송 지연 여부
			@param[in] delay 최대 전송 지연 시간
			
			@since 1.0.0
			*/
			void setDelay(bool useDelay, DWORD delay);

			///@}

		public:
			///frontend or backend side Session
			std::shared_ptr<rgs::io::Session> pairSession;

		private:
			///연결이 수립될 때 부여받는 id
			unsigned int id_;					
			
			///패킷을 처리할 Protocol 객체
			std::shared_ptr<rgs::protocol::Protocol> protocol_;
			
			///패킷 큐
			concurrency::concurrent_queue<rgs::protocol::Packet*> packets_;
			
			///wrapping한 Socket
			std::shared_ptr<rgs::io::Socket> socket_;

			///handler
			rgs::cb::Callback<rgs::io::ConnectionEvent, std::shared_ptr<Session>> handler_;
		};

		template<class EventHandler>
		void Session::registerHandler(EventHandler&& handler)
		{
			handler_.setCallback(std::forward<EventHandler>(handler));
		}

		/**
		@class CreateSession

		@brief Session의 생성과 초기화를 하는 팩토리 클래스

		Session의 구체적인 생성과 초기화를 정의한다.

		@author 조민우
		@since 1.0.0
		*/
		class CreateSession
		{
		public:
			/** 
			@brief 디폴트 생성자

			@param[in] protocol 패킷을 만들 객체

			@since 1.0.0
			*/
			explicit CreateSession(std::shared_ptr<rgs::protocol::Protocol> protocol);

			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~CreateSession() = default;

		public:
			/**
			@brief Session의 새로운 인스턴스를 얻는다.

			@return 새로운 Session

			@since 1.0.0
			*/
			virtual std::shared_ptr<Session> operator()();

		private:
			std::shared_ptr<rgs::protocol::Protocol> protocol_;
		};
	}
}