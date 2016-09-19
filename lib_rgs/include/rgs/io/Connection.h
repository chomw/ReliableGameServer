/**
@file Connection.h
@author 조민우(cmw1728@naver.com)
@since 1.0.0
*/

#pragma once
#include <memory>
#include <ppl.h>
#include <concurrent_queue.h>
#include <concurrent_vector.h>

#include "../callback/Callbacks.h"

namespace rgs
{
	namespace io
	{
		enum class ConnectionEvent;

		struct IPAddress;
		class Socket;
		class Session;
		class CreateSession;

		/**
		@class Connection

		@brief Session 관리 및 Session의 Connection Event를 처리하는 클래스

		(1) Session의 Connection Event가 발생하면 등록된 커넥션 이벤트 핸들러를 호출한다.
		(2) Session을 관리한다.

		@author 조민우
		@since 1.0.0
		*/
		class Connection
		{
		private:
			struct ConnectionEvent
			{
				ConnectionEvent() = default;
				ConnectionEvent(rgs::io::ConnectionEvent connectionEvent, std::shared_ptr<rgs::io::Session> session):
					connectionEvent(connectionEvent),
					session(session) {}

				rgs::io::ConnectionEvent connectionEvent;
				std::shared_ptr<rgs::io::Session> session;
			};

		public:
			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			virtual ~Connection() = default;
			
		public:
			/**
			@brief Connection의 내부 로직을 수행한다.

			@since 1.0.0
			*/
			void update();

			/**
			@brief 연결된 세션의 수를 얻는다.

			@return 연결된 세션 수

			@since 1.0.0
			*/
			unsigned int connectedSize()const;

			/**
			@brief 커넥션 이벤트 핸들러를 등록한다.

			@param[in] connectionEvent 커넥션 이벤트
			@param[in] handler 커넥션 이벤트 발생시 호출될 핸들러

			@since 1.0.0
			*/
			template<class EventHandler>
			void registerHandler(rgs::io::ConnectionEvent connectionEvent, EventHandler&& handler);		

		protected:
			/**
			@brief 커넥션 이벤트를 발생시킨다

			@param[in] connectionEvent 커넥션 이벤트

			@since 1.0.0
			*/
			void complete(rgs::io::ConnectionEvent connectionEvent, std::shared_ptr<rgs::io::Session> session);

			/**
			@brief Connected 커넥션 이벤트 발생시 호출된다.

			@param[in] session 커넥션 이벤트가 발생한 Session

			@since 1.0.0
			*/
			virtual void connected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief Disconnected 커넥션 이벤트 발생시 호출된다.

			@param[in] session 커넥션 이벤트가 발생한 Session

			@since 1.0.0
			*/
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

		private:
			/**
			@brief Session의 패킷 핸들러를 호출한다.

			@since 1.0.0
			*/
			void dispatch();

		private:
			std::atomic<unsigned int> connectedSize_ = 0;
			std::map<int, std::shared_ptr<Session>> connectedSessions_;

			rgs::cb::Callbacks<std::shared_ptr<rgs::io::Session>> handlers_;
			concurrency::concurrent_queue<Connection::ConnectionEvent> connectionEventQueue_;
		};

		template<class EventHandler>
		void Connection::registerHandler(rgs::io::ConnectionEvent connectionEvent, EventHandler&& handler)
		{
			handlers_.add((int)connectionEvent, std::forward<EventHandler>(handler));
		}

		/**
		@class Listener

		@brief 특정 포트로 연결 대기하는 세션 풀을 구현한 클래스

		(1) Session의 Connection Event가 발생하면 등록된 커넥션 이벤트 핸들러를 호출한다.
		(2) 연결 대기(listening)하도록 세션 풀을 관리한다.

		@author 조민우
		@since 1.0.0
		*/
		class Listener : public Connection
		{
		public:
			enum { MAX_SIZE = 13000, CAPACITY = 1000};

		public:
			/**
			@brief Listener를 개시(initiate)한다.

			@param[in] createSession session을 생성할 팩토리
			@param[in] port 연결 대기할 포트

			@return 개시하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool initiate(std::shared_ptr<rgs::io::CreateSession> createSession, int port);

			/**
			@brief 세션 풀의 capacity를 설정한다.

			@param[in] capacity 세션 풀의 capacity

			@since 1.0.0
			*/
			void setCapacity(unsigned int capacity);

			/**
			@brief 세션 풀의 최대 크기를 설정한다.

			@param[in] maxSize 세션 풀의 최대 크기

			@since 1.0.0
			*/
			void setMaxSize(unsigned int maxSize);

		private:
			/**
			@brief Connected 커넥션 이벤트 발생시 호출된다.

			@param[in] session 커넥션 이벤트가 발생한 Session

			@since 1.0.0
			*/
			virtual void connected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief Disconnected 커넥션 이벤트 발생시 호출된다.

			@param[in] session 커넥션 이벤트가 발생한 Session

			@since 1.0.0
			*/
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief 설정된 capacity만큼 세션 풀의 크기를 유지한다.

			@since 1.0.0
			*/
			void keepCapacity();

		private:
			int port_ = 0;

			unsigned int size_ = 0;
			std::atomic<unsigned int> capacity_ = 0;
			std::atomic<unsigned int> maxSize_ = 0;
			
			std::shared_ptr<rgs::io::CreateSession> createSession_;
			concurrency::concurrent_vector<std::shared_ptr<rgs::io::Session>> sessions_;
		};
		
		/**
		@class Connector

		@brief 특정 포트로 연결 요청하는 세션 풀을 구현한 클래스

		(1) Session의 Connection Event가 발생하면 등록된 커넥션 이벤트 핸들러를 호출한다.
		(2) 연결 요청(connect)하도록 세션 풀을 관리한다.

		@author 조민우
		@since 1.0.0
		*/
		class Connector : public Connection
		{
		public:
			/**
			@brief Connector를 개시(initiate)한다.

			@param[in] createSession session을 생성할 팩토리
			@param[in] endPointIp 연결할 remote host 주소

			@return 개시하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool initiate(std::shared_ptr<rgs::io::CreateSession> createSession, const rgs::io::IPAddress& endPointIp);

			/**
			@brief 연결 요청한 Session을 얻는다.

			세션 풀에서 세션을 얻어서 endpoint로 connect한 후 리턴한다.

			@return 연결 요청한 Session

			@since 1.0.0
			*/
			std::shared_ptr<rgs::io::Session> getSession();

		private:
			/**
			@brief Disconnected 커넥션 이벤트 발생시 호출된다.

			@param[in] session 커넥션 이벤트가 발생한 Session

			@since 1.0.0
			*/
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief 연결 종료된 세션을 세션 풀에 반납한다.

			@param[in] session 반납할 세션

			@since 1.0.0
			*/
			void returnSession(std::shared_ptr<rgs::io::Session> session);

		private:
			std::shared_ptr<rgs::io::CreateSession> createSession_;
			std::shared_ptr<rgs::io::IPAddress> endPointIp_;

			concurrency::concurrent_queue<std::shared_ptr<rgs::io::Session>> waitingSessions_;
		};
	}
}