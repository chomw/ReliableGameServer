#pragma once
#include <memory>
#include <map>
#include <vector>
#include <atomic>
#include <ppl.h>
#include <concurrent_unordered_map.h>

namespace rgs
{
	enum class CORE;

	namespace io
	{
		namespace proactor{
			class Proactor;
			class Acceptor;
			class Disconnector;
			class Sender;
			class Receiver;
			class Connector;
		}
		
		class Socket;
		class ListenSocket;

		class NetworkEngine
		{
		public:
			NetworkEngine();

			virtual ~NetworkEngine();
			static NetworkEngine& instance();
		
		public:
			void start();
			void stop();

		public:
			void registerSocket(std::shared_ptr<rgs::io::Socket> socket);
			void setCore(rgs::Core core);

			std::shared_ptr<ListenSocket> getListenSocket(int port);
			rgs::Core getCore()const;


		private:
			static std::unique_ptr<NetworkEngine> instance_;

		protected:
			concurrency::concurrent_unordered_map<int, std::shared_ptr<ListenSocket>> listenSockets_;

			std::unique_ptr<rgs::io::proactor::Proactor>		proactor_;
			std::unique_ptr<rgs::io::proactor::Acceptor>		acceptor_;
			std::unique_ptr<rgs::io::proactor::Receiver>		receiver_;
			std::unique_ptr<rgs::io::proactor::Sender>			sender_;
			std::unique_ptr<rgs::io::proactor::Disconnector>	disconnector_;
			std::unique_ptr<rgs::io::proactor::Connector>		connector_;

			std::atomic<rgs::Core> core_;
		};
	}
}