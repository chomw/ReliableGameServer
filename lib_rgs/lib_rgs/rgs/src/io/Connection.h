#pragma once
#include <memory>
#include <ppl.h>
#include <concurrent_queue.h>
#include <concurrent_vector.h>

#include "../callback/Callbacks.h"

namespace rgs
{
	namespace protocol
	{
		class Protocol;
	}

	namespace io
	{
		enum class IoEvent;

		struct IPAddress;
		class Socket;
		class Session;
		
		class Connection
		{
		protected:
			struct IoEvent
			{
				IoEvent() = default;
				IoEvent(rgs::io::IoEvent ioEvent, std::shared_ptr<rgs::io::Session> session):
					ioEvent(ioEvent),
					session(session) {}

				rgs::io::IoEvent ioEvent;
				std::shared_ptr<rgs::io::Session> session;
			};

		public:
			explicit Connection(int port = 0);

			virtual ~Connection() = default;
			
			virtual void update();

			virtual std::shared_ptr<rgs::io::Session> getSession();

		public:
			unsigned int sessionSize()const;

			unsigned int connectedSize()const;

			virtual unsigned int waitingSize()const;

		public:
			//handler등록
			template<class EventHandler>
			void registerHandler(rgs::io::IoEvent ioEvent, EventHandler&& handler);

			//event 발생을 알림
			virtual void complete(rgs::io::IoEvent ioEvent, std::shared_ptr<rgs::io::Session> session);

		private:
			void dispatch();

			void connected(std::shared_ptr<rgs::io::Session>);

			void disconnected(std::shared_ptr<rgs::io::Session>);

		protected:
			virtual std::shared_ptr<Session> createSession() = 0;		

		protected:
			const int port_ = 0;

			std::atomic<unsigned int> sessionSize_ = 0;
			std::atomic<unsigned int> connectedSize_ = 0;
			std::atomic<unsigned int> capacity_ = 0;
			std::atomic<unsigned int> maxConnection_ = 0;

			std::shared_ptr<rgs::protocol::Protocol> protocol_;

			std::map<int, std::shared_ptr<Session>> connectedSessions_;
			concurrency::concurrent_vector<std::shared_ptr<rgs::io::Session>> sessions_;

		private:
			rgs::cb::Callbacks<std::shared_ptr<rgs::io::Session>> handlers_;
			concurrency::concurrent_queue<Connection::IoEvent> ioEventQueue_;
		};

		template<class EventHandler>
		void Connection::registerHandler(rgs::io::IoEvent ioEvent, EventHandler&& handler)
		{
			handlers_.add((int)ioEvent, std::forward<EventHandler>(handler));
		}

		class Listener : public Connection
		{
		public:
			explicit Listener(int port);

			virtual void complete(rgs::io::IoEvent ioEvent, std::shared_ptr<rgs::io::Session> session);
			
			bool initialize(std::shared_ptr<rgs::protocol::Protocol> protocol, unsigned int capacity, unsigned int maxConnection);

		private:
			virtual std::shared_ptr<Session> createSession();
		};
		
		class Connector : public Connection
		{
		public:
			explicit Connector(const rgs::io::IPAddress& endPointIp);

		public:
			virtual bool initialize(std::shared_ptr<rgs::protocol::Protocol> protocol);

			virtual std::shared_ptr<rgs::io::Session> getSession();

			virtual unsigned int waitingSize()const;

		private:
			virtual std::shared_ptr<Session> createSession();
			
			void returnSession(std::shared_ptr<rgs::io::Session> session);

		private:
			std::shared_ptr<rgs::io::IPAddress> endPointIp_;
			concurrency::concurrent_queue<std::shared_ptr<rgs::io::Session>> waitingSessions_;
		};
	}
}