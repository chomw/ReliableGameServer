#pragma once
#include <atomic>
#include "../protocol/Protocol.h"

#include <ppl.h>
#include <concurrent_queue.h>

namespace rgs
{
	namespace io
	{
		enum class IoEvent;
		
		class Socket;
		class Connection;

		class Session : public std::enable_shared_from_this<Session>
		{
		public:
			Session();
			Session(rgs::io::Connection*);
			virtual ~Session();

		public:
			bool initialize();
			bool initialize(std::shared_ptr<rgs::protocol::Protocol> protocol, int port = -1);

			bool write(const rgs::protocol::RowData& rowData);

			unsigned int read(const rgs::protocol::RowData& rowData, unsigned int& readBytes);

			bool bind();

			bool connect(const std::string& address, int port, const rgs::protocol::RowData* rowData = nullptr);

			bool disconnect();

			bool listening();

			void dispatch();

			void complete(rgs::io::IoEvent ioEvent);

		public:
			unsigned int id()const
			{
				return id_;
			}

			bool isConnected()const;
			
			void useKeepAlive(bool);

			void setKeepAliveInterval(DWORD);

			void ipAddress(const rgs::io::IPAddress&);

			rgs::io::IPAddress ipAddress()const;

			void setDelay(bool useDelay);

			void setDelay(bool useDelay, DWORD delay);

		public:
			std::shared_ptr<rgs::io::Session> pairSession;

		private:
			unsigned int id_;
			
			rgs::io::IPAddress ipAddress_;
			rgs::io::Connection* connection_;

			std::atomic<unsigned int> packetSize_;
			std::shared_ptr<rgs::protocol::Protocol> protocol_;
			concurrency::concurrent_queue<rgs::protocol::Packet*> packets_;
			
			std::shared_ptr<rgs::io::Socket> socket_;
		};
	}
}