/**
@file NetworkEngine.h
@author ���ο�(cmw1728@naver.com)
@since 1.0.0
*/

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

		/**
		@class NetworkEngine

		@brief Proactor ������ Proactive Initiator ������ �����Ѵ�.

		Proactor ��ü�鿡 ���ؼ� �񵿱� ��Ƽ������ ��Ʈ��ũ IO�� ó���ȴ�. 

		(1) Proactor�� ����(Initiate)�Ѵ�.
		(2) Listen Socket�� �����Ѵ�.
		(3) Socket���� Proactor ��ü���� �����Ѵ�.

		@author ���ο�
		@since 1.0.0
		*/
		class NetworkEngine
		{
		public:
			/**
			@brief ����Ʈ ������

			@since 1.0.0
			*/
			NetworkEngine();

		public:
			/**
			@brief Proactor�� �����Ѵ�.

			@since 1.0.0
			*/
			void start();

			/**
			@brief Proactor�� �����.

			@since 1.0.0
			*/
			void stop();

		public:
			/**
			@brief ���Ͽ� Proactor ��ü�� �����Ͽ� Proactor�� ����Ѵ�.

			@param[in] socket Proactor ��ü���� �����Ϸ��� ����

			@since 1.0.0
			*/
			void registerSocket(std::shared_ptr<rgs::io::Socket> socket);

			/**
			@brief Listen socket�� ��´�.

			@param[in] port �ش� ��Ʈ�� ���� ������� Listen Socket�� ��´�.

			@return Listen Socket

			@since 1.0.0
			*/
			std::shared_ptr<ListenSocket> getListenSocket(int port);

			/**
			@brief Singleton ��ü

			@since 1.0.0
			*/
			static NetworkEngine& instance();

		private:
			concurrency::concurrent_unordered_map<int, std::shared_ptr<ListenSocket>> listenSockets_;

			std::unique_ptr<rgs::io::proactor::Proactor>		proactor_;
			std::unique_ptr<rgs::io::proactor::Acceptor>		acceptor_;
			std::unique_ptr<rgs::io::proactor::Receiver>		receiver_;
			std::unique_ptr<rgs::io::proactor::Sender>			sender_;
			std::unique_ptr<rgs::io::proactor::Disconnector>	disconnector_;
			std::unique_ptr<rgs::io::proactor::Connector>		connector_;

			static std::unique_ptr<NetworkEngine> instance_;
		};
	}
}