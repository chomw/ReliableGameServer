/**
@file Connection.h
@author ���ο�(cmw1728@naver.com)
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

		@brief Session ���� �� Session�� Connection Event�� ó���ϴ� Ŭ����

		(1) Session�� Connection Event�� �߻��ϸ� ��ϵ� Ŀ�ؼ� �̺�Ʈ �ڵ鷯�� ȣ���Ѵ�.
		(2) Session�� �����Ѵ�.

		@author ���ο�
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
			@brief ����Ʈ �Ҹ���

			@since 1.0.0
			*/
			virtual ~Connection() = default;
			
		public:
			/**
			@brief Connection�� ���� ������ �����Ѵ�.

			@since 1.0.0
			*/
			void update();

			/**
			@brief ����� ������ ���� ��´�.

			@return ����� ���� ��

			@since 1.0.0
			*/
			unsigned int connectedSize()const;

			/**
			@brief Ŀ�ؼ� �̺�Ʈ �ڵ鷯�� ����Ѵ�.

			@param[in] connectionEvent Ŀ�ؼ� �̺�Ʈ
			@param[in] handler Ŀ�ؼ� �̺�Ʈ �߻��� ȣ��� �ڵ鷯

			@since 1.0.0
			*/
			template<class EventHandler>
			void registerHandler(rgs::io::ConnectionEvent connectionEvent, EventHandler&& handler);		

		protected:
			/**
			@brief Ŀ�ؼ� �̺�Ʈ�� �߻���Ų��

			@param[in] connectionEvent Ŀ�ؼ� �̺�Ʈ

			@since 1.0.0
			*/
			void complete(rgs::io::ConnectionEvent connectionEvent, std::shared_ptr<rgs::io::Session> session);

			/**
			@brief Connected Ŀ�ؼ� �̺�Ʈ �߻��� ȣ��ȴ�.

			@param[in] session Ŀ�ؼ� �̺�Ʈ�� �߻��� Session

			@since 1.0.0
			*/
			virtual void connected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief Disconnected Ŀ�ؼ� �̺�Ʈ �߻��� ȣ��ȴ�.

			@param[in] session Ŀ�ؼ� �̺�Ʈ�� �߻��� Session

			@since 1.0.0
			*/
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

		private:
			/**
			@brief Session�� ��Ŷ �ڵ鷯�� ȣ���Ѵ�.

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

		@brief Ư�� ��Ʈ�� ���� ����ϴ� ���� Ǯ�� ������ Ŭ����

		(1) Session�� Connection Event�� �߻��ϸ� ��ϵ� Ŀ�ؼ� �̺�Ʈ �ڵ鷯�� ȣ���Ѵ�.
		(2) ���� ���(listening)�ϵ��� ���� Ǯ�� �����Ѵ�.

		@author ���ο�
		@since 1.0.0
		*/
		class Listener : public Connection
		{
		public:
			enum { MAX_SIZE = 13000, CAPACITY = 1000};

		public:
			/**
			@brief Listener�� ����(initiate)�Ѵ�.

			@param[in] createSession session�� ������ ���丮
			@param[in] port ���� ����� ��Ʈ

			@return �����ϸ� true, �����ϸ� false

			@since 1.0.0
			*/
			bool initiate(std::shared_ptr<rgs::io::CreateSession> createSession, int port);

			/**
			@brief ���� Ǯ�� capacity�� �����Ѵ�.

			@param[in] capacity ���� Ǯ�� capacity

			@since 1.0.0
			*/
			void setCapacity(unsigned int capacity);

			/**
			@brief ���� Ǯ�� �ִ� ũ�⸦ �����Ѵ�.

			@param[in] maxSize ���� Ǯ�� �ִ� ũ��

			@since 1.0.0
			*/
			void setMaxSize(unsigned int maxSize);

		private:
			/**
			@brief Connected Ŀ�ؼ� �̺�Ʈ �߻��� ȣ��ȴ�.

			@param[in] session Ŀ�ؼ� �̺�Ʈ�� �߻��� Session

			@since 1.0.0
			*/
			virtual void connected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief Disconnected Ŀ�ؼ� �̺�Ʈ �߻��� ȣ��ȴ�.

			@param[in] session Ŀ�ؼ� �̺�Ʈ�� �߻��� Session

			@since 1.0.0
			*/
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief ������ capacity��ŭ ���� Ǯ�� ũ�⸦ �����Ѵ�.

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

		@brief Ư�� ��Ʈ�� ���� ��û�ϴ� ���� Ǯ�� ������ Ŭ����

		(1) Session�� Connection Event�� �߻��ϸ� ��ϵ� Ŀ�ؼ� �̺�Ʈ �ڵ鷯�� ȣ���Ѵ�.
		(2) ���� ��û(connect)�ϵ��� ���� Ǯ�� �����Ѵ�.

		@author ���ο�
		@since 1.0.0
		*/
		class Connector : public Connection
		{
		public:
			/**
			@brief Connector�� ����(initiate)�Ѵ�.

			@param[in] createSession session�� ������ ���丮
			@param[in] endPointIp ������ remote host �ּ�

			@return �����ϸ� true, �����ϸ� false

			@since 1.0.0
			*/
			bool initiate(std::shared_ptr<rgs::io::CreateSession> createSession, const rgs::io::IPAddress& endPointIp);

			/**
			@brief ���� ��û�� Session�� ��´�.

			���� Ǯ���� ������ �� endpoint�� connect�� �� �����Ѵ�.

			@return ���� ��û�� Session

			@since 1.0.0
			*/
			std::shared_ptr<rgs::io::Session> getSession();

		private:
			/**
			@brief Disconnected Ŀ�ؼ� �̺�Ʈ �߻��� ȣ��ȴ�.

			@param[in] session Ŀ�ؼ� �̺�Ʈ�� �߻��� Session

			@since 1.0.0
			*/
			virtual void disconnected(std::shared_ptr<rgs::io::Session> session);

			/**
			@brief ���� ����� ������ ���� Ǯ�� �ݳ��Ѵ�.

			@param[in] session �ݳ��� ����

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