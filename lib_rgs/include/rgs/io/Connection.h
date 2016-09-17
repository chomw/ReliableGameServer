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

		class Connection
		{
		protected:
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
			virtual ~Connection() = default;
			
		public:
			void update();

			unsigned int connectedSize()const;

			//handlerµî·Ï
			template<class EventHandler>
			void registerHandler(rgs::io::ConnectionEvent connectionEvent, EventHandler&& handler);		

		protected:
			void complete(rgs::io::ConnectionEvent connectionEvent, std::shared_ptr<rgs::io::Session> session);

			virtual void connected(std::shared_ptr<rgs::io::Session> session);
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

		private:
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

		class Listener : public Connection
		{
		public:
			enum { MAX_SIZE = 13000, CAPACITY = 1000};

		public:
			bool initiate(std::shared_ptr<rgs::io::CreateSession> createSession, int port);

			void setCapacity(unsigned int capacity);
			void setMaxSize(unsigned int maxSize);

		private:
			virtual void connected(std::shared_ptr<rgs::io::Session> session);
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

			void keepCapacity();

		private:
			int port_ = 0;

			unsigned int size_ = 0;
			std::atomic<unsigned int> capacity_ = 0;
			std::atomic<unsigned int> maxSize_ = 0;
			
			std::shared_ptr<rgs::io::CreateSession> createSession_;
			concurrency::concurrent_vector<std::shared_ptr<rgs::io::Session>> sessions_;
		};
		
		class Connector : public Connection
		{
		public:
			bool initiate(std::shared_ptr<rgs::io::CreateSession> createSession, const rgs::io::IPAddress& endPointIp);

			std::shared_ptr<rgs::io::Session> getSession();

		private:
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

			void returnSession(std::shared_ptr<rgs::io::Session> session);

		private:
			std::shared_ptr<rgs::io::CreateSession> createSession_;
			std::shared_ptr<rgs::io::IPAddress> endPointIp_;

			concurrency::concurrent_queue<std::shared_ptr<rgs::io::Session>> waitingSessions_;
		};
	}
}