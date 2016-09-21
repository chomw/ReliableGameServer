/**
@file NetworkEngine.h
@author 조민우(cmw1728@naver.com)
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

		@brief Proactor 패턴의 Proactive Initiator 역할을 수행한다.

		Proactor 객체들에 의해서 비동기 멀티스레딩 네트워크 IO가 처리된다. 

		(1) Proactor를 개시(Initiate)한다.
		(2) Listen Socket을 관리한다.
		(3) Socket에게 Proactor 객체들을 설정한다.

		@author 조민우
		@since 1.0.0
		*/
		class NetworkEngine
		{
		public:
			/**
			@brief 디폴트 생성자

			@since 1.0.0
			*/
			NetworkEngine();

		public:
			/**
			@brief Proactor를 시작한다.

			@since 1.0.0
			*/
			void start();

			/**
			@brief Proactor를 멈춘다.

			@since 1.0.0
			*/
			void stop();

		public:
			/**
			@brief 소켓에 Proactor 객체를 설정하여 Proactor에 등록한다.

			@param[in] socket Proactor 객체들을 설정하려는 소켓

			@since 1.0.0
			*/
			void registerSocket(std::shared_ptr<rgs::io::Socket> socket);

			/**
			@brief Listen socket을 얻는다.

			@param[in] port 해당 포트로 연결 대기중인 Listen Socket을 얻는다.

			@return Listen Socket

			@since 1.0.0
			*/
			std::shared_ptr<ListenSocket> getListenSocket(int port);

			/**
			@brief Singleton 객체

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