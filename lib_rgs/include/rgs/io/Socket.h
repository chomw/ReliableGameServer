/**
@file Socket.h
@author 조민우(cmw1728@naver.com)
@since 1.0.0
*/
#pragma once
#include <memory>

#include "proactor/Act.h"
#include "../thread/Synchronous.h"

namespace rgs
{
	namespace io
	{
		namespace proactor
		{
			class Proactor;
			class Acceptor;
			class Disconnector;
			class Sender;
			class Receiver;
			class Connector;
		}
		
		enum class ConnectionEvent;
		class ListenSocket;
		class Session;

		/**
		@class Socket

		@brief 엔진 Level에서 IO를 수행하는 클래스.

		(1) winsock2 row socket Wrapper :
		winsock2을 이용해 tcp/ip 통신을 수행합니다.

		(2) 고성능 IO : 
		송신 버퍼링을 통한 성능 최적화

		@author 조민우
		@since 1.0.0
		*/
		class Socket
		{
		public:
			enum ACT_TYPE
			{
				ACT_SEND = 0,
				ACT_RECV,
				ACT_ACCEPT,
				ACT_DISCONNECT,
				ACT_CONNECT,
				ACT_TYPE_CNT
			};

			enum 
			{ 
				DEFAULT_DELAY = 10,			//전송 딜레이
				IMMEDIATELY_SEND = 2		//전송 딜레이 없이 바로 송신할 횟수
			};
		
			enum State
			{
				CONNECTED,
				CONNECTING,
				DISCONNECTED,
				DISCONNECTING,
				LISTENING
			};

		public:
			/// @name Constructors and Destructor
			/// @{

			/** 
			@brief 디폴트 생성자

			Session을 참조자로 받아서 커넥션 이벤트나 데이터 수신시 Session에게 전달한다.
			순환 참조를 방지하기 위해 참조자로 가지게 한다.

			@param[in] session 응용 레벨의 Session 객체
			
			@since 1.0.0
			*/
			Socket(Session& session);
			
			/** 
			@brief 소멸자

			데이터 멤버를 소멸한다.

			@since 1.0.0
			*/
			virtual ~Socket();

			///@}

		public:
			/// @name IO Functions
			/// @{

			/** 
			@brief RowSocket을 생성한다.

			winsock2 row socket을 생성한다.

			@return row socket 생성했으면 true, 실패했으면 false

			@since 1.0.0
			*/
			bool createRowSocket();

			/**
			@brief 전송 요청을 한다.

			전송 지연에 의해 즉시 송신되지 않을 수 있다.

			@param[in] data 전송할 데이터
			@param[in] size 전송할 데이터의 크기

			@return 전송 요청이 성공했으면 true, 아니면 false

			@since 1.0.0
			*/
			bool write(BYTE* data, unsigned int size);
			
			/**
			@brief 비동기 수신을 요청한다.

			네트워크 엔진단에서 비동기 수신을 요청한다.

			@return 비동기 수신 요청이 성공했으면 true, 아니면 false

			@since 1.0.0
			*/
			bool startReceive();

			/**
			@brief 데이터 수신을 처리한다.

			비동기 수신 요청이 완료되면 실제 데이터 수신을 처리한다.

			@param[in] receiveBytes 비동기로 수신한 데이터의 크기

			@return 데이터 수신을 성공하면 true, 아니면 false

			@since 1.0.0
			*/
			bool receive(unsigned int receiveBytes);

			/** 
			@brief 소켓에 로컬 주소를 부여한다.

			@return 바인딩이 성공하면 true, 아니면 false

			@since 1.0.0
			*/
			bool bind();

			/**
			@brief remote host로 연결한다.

			@param[in] address IPv4 remote host ip
			@param[in] port remote host port
			@param[in] data 초기 전송할 데이터
			@param[in] size 초기 전송할 데이터의 크기

			@return 연결 요청이 성공하면 true, 아니면 false

			@since 1.0.0
			*/
			bool connect(const std::string& address, USHORT port, const BYTE* data = nullptr, unsigned int size = 0);
			
			/**
			@brief 연결을 종료한다.

			@return 성공적으로 연결 종료을 하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool disconnect();
			
			/**
			@brief 연결 대기한다.

			@return 성공적으로 연결 대기하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool listening();

			/**
			@brief 커넥션 이벤트를 발생시킨다

			네트워크 엔진에 의해 커넥션 이벤트가 발생된다.
			내부적으로 커넥션 이벤트를 처리한 후 Session에게 전달한다.

			@param[in] connectionEvent 커넥션 이벤트

			@since 1.0.0
			*/
			void complete(rgs::io::ConnectionEvent connectionEvent);

			/**
			@brief 연결됐지만 완료 통지가 발생하지 않았으면 연결시킨다.

			@return 연결되면 true, 아니면 false

			@since 1.0.0
			*/
			bool completeListening();
			
			/**
			@brief 지연된 전송을 전송한다.

			write()를 지연해서 패킷을 뭉친 게 있으면 전송시킨다.

			@return 전송되면 true, 아니면 false

			@since 1.0.0
			*/
			void sendDelayed();

			///@}

		public:

			/// @name Data Access And Inspection
			/// @{

			/**
			@brief 연결됐는지 여부를 저장한다.

			@param[in] connected 연결됐는지 여부

			@since 1.0.0
			*/
			void setConnected(bool connected);

			/**
			@brief proactor를 초기화한다.

			비동기 IO를 위한 proactor객체들을 초기화한다.

			@param[in] proactor proactor 패턴의 Completion Dispatcher
			@param[in] acceptor 비동기 연결 수락을 위한 Completion Handler
			@param[in] disconnector 비동기 연결 종료를 위한 Completion Handler
			@param[in] sender 비동기 송신을 위한 Completion Handler
			@param[in] receiver 비동기 수신을 위한 Completion Handler
			@param[in] connector 비동기 연결 요청을 위한 Completion Handler

			@since 1.0.0
			*/
			void setProactor(
				rgs::io::proactor::Proactor* proactor
				, rgs::io::proactor::Acceptor* acceptor
				, rgs::io::proactor::Disconnector* disconnector
				, rgs::io::proactor::Sender* sender
				, rgs::io::proactor::Receiver* receiver
				, rgs::io::proactor::Connector* connector
			);
			
			/** 
			@brief 연결 대기 요청할 listen socket을 초기화한다.

			@param[in] listenSocket 연결 대기 소켓

			@since 1.0.0
			*/
			void setListenSocket(std::shared_ptr<ListenSocket> listenSocket);

			/**
			@brief winsock2 row socket을 얻는다.

			@return winsock2 row socket

			@since 1.0.0
			*/
			::SOCKET					getRowSocket()const;
		
			/**
			@brief winsock2 row socket이 IOCP에 등록됐는지 여부

			@return winsock2 row socket이 IOCP에 등록됐으면 true, 아니면 false

			@since 1.0.0
			*/
			bool						getRegisterToIocp()const;
			
			/**
			@brief 연결됐는지 여부

			@return 연결되었으면 true, 연결되지 않았으면 false

			@since 1.0.0
			*/
			bool isConnected()const;
			
			/**
			@brief winsock2 row socket이 유효한 소켓인지 여부

			@return winsock2 row socket이 유효한 소켓이면 true, 아니면 false

			@since 1.0.0
			*/
			bool isValidRowSocket()const;

			/**
			@brief 세션의 timeout될 시간을 설정한다.

			지정된 timeout 동안 IO가 없으면 연결이 종료된다.

			@param[in] timeout timeout될 시간 간격. 0을 지정하면 timeout되지 않는다.

			@since 1.0.0
			*/
			void setTimeout(DWORD timeout);
			
			/** 
			@brief 현재 timeout 됐는지 여부

			@return 현재 timeout 됐으면 true, 아니면 false

			@since 1.0.0
			*/
			bool isTimeout()const;			

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

			/**
			@brief 연결된 remote host의 주소를 얻는다.

			IPv4 ip, port를 얻는다.

			@return ip, port 주소

			@since 1.0.0
			*/
			rgs::io::IPAddress ipAddress()const;

			///@}

		private:
			/**
			@brief 내부 송수신 버퍼를 초기화하낟.

			@since 1.0.0
			*/
			void initialize();

			/**
			@brief 실제로 전송한다.

			@param[in] data 실제로 전송할 데이터
			@param[in] size 실제로 전송할 데이터의 크기

			@return 실제로 전송이 성공하면 true, 실패하면 false

			@since 1.0.0
			*/
			bool send(BYTE* data, unsigned int size);

			/**
			@brief IOCP 등록 여부를 저장한다.

			@param[in] registerToIocp IOCP에 등록되었는지 여부

			@since 1.0.0
			*/
			void setRegisterToIocp(bool registerToIocp);

			/**
			@brief remote host의 ip, port를 저장한다.

			@return remote host의 주소를 성공적으로 읽었으면 true, 아니면 false

			@since 1.0.0
			*/
			bool readAddress();

			/**
			@brief 마지막 IO를 수행한 시간을 저장한다.

			마지막 IO를 수행한 시간을 저장한다.
			마지막 IO를 수행하고 나서 지정한 timeout이 되도록 IO가 없으면 연결이 종료된다.

			@param[in] time 현재 시간

			@since 1.0.0
			*/
			void setLastIoTime(DWORD time);

		public:
			static LPFN_CONNECTEX					ConnectEx;			//ConnectEx의 함수 포인터

		private:
			mutable rgs::thread::sync::SimpleLock	writeLock_;			//write(), sendBuffer() lock
			mutable rgs::thread::sync::SimpleLock	connectionLock_;	//disconnect(), complete() lock

			rgs::io::proactor::Proactor*			proactor_;			//proactors
			rgs::io::proactor::Act acts_[ACT_TYPE_CNT];

			std::atomic<::SOCKET>					rowSocket_;			//winsock row socket
			std::atomic<bool>						isConnect_;			//연결 여부
			std::atomic<bool>						isRegisterToIocp_;	//iocp 등록 여부
			std::atomic<bool>						isCanceled_;		//연결 완료 전에 취소했는지 여부
			std::atomic<bool>						useDelay_;			//delay 전송 사용여부

			unsigned int delay_;			//전송 지연 시간
			unsigned int writeCount_;		//전송 요청 횟수
			unsigned int receiveSize_;		//수신 버퍼 크기
			unsigned int sendSize_;			//송신 버퍼 크기

			DWORD lastSendTime_;						//마지막 실제 전송 시간
			DWORD lastCheckTime_;						//마지막 delayTime_ 계산 시간
			std::atomic<DWORD> checkTime_;				//delayTime_ 계산 간격
 
			std::atomic<DWORD> timeout_;		//timeout 시간
			std::atomic<DWORD> lastIoTime_;		//마지막 io 시간

			std::atomic<Socket::State> currentState_;	//소켓 상태
			std::atomic<Socket::State> operatingState_;	//소켓 동작 상태

			BYTE receiveBuffer_[rgs::SocketConfig::BUFFER_SIZE];	//수신 버퍼
			BYTE sendBuffer_[rgs::SocketConfig::BUFFER_SIZE];		//송신 버퍼

			std::shared_ptr<ListenSocket> listenSocket_;		//listen 소켓
			Session& session_;									//세션											
			rgs::io::IPAddress ipAddress_;					///remote host의 ip, port
		};

		/**
		@class ListenSocket

		@brief 연결 대기 중인 listen socket

		연결 대기를 하다가 연결 요청이 오면 Accept Socket과 연결시킵니다.

		@author 조민우
		@since 1.0.0
		*/
		class ListenSocket
		{
		public:
			/**
			@brief 초기화를 한다.

			@param[in] proactor proactor 패턴의 Completion Dispatcher
			@param[in] port 연결 대기할 port

			@since 1.0.0
			*/
			void initialize(rgs::io::proactor::Proactor* proactor, unsigned int port);

			/**
			@brief 연결 수락할 소켓을 추가한다.

			@param[in] acceptSocket 연결 수락할 소켓
			@param[out] buffer 초기 수신 데이터를 받을 버퍼
			@param[in] buffer의 크기
			@param[in] 비동기 연결 수락을 위한 completion handler

			@return 연결 수락 요청이 성공하면 true, 아니면 false

			@since 1.0.0
			*/
			bool listen(
				Socket* acceptSocket
				, char* buffer
				, unsigned int bufferSize
				, rgs::io::proactor::Act* act
			);

			/**
			@brief listen socket의 winsock2 row socket

			@return winsock2 row socket

			@since 1.0.0
			*/
			::SOCKET getRowSocket()const;

		private:
			::SOCKET rowSocket_;
		};

		/**
		@class SocketThread

		@brief Socket의 백그라운드 로직을 수행하는 클래스

		(1) 전송 지연 :
		최대 전송 지연 시간 내에 지연된 Socket의 전송을 전송하도록 보장한다.

		(2) timeout :
		timeout됐는지를 확인하고 됐으면 연결을 종료한다.

		@author 조민우
		@since 1.0.0
		*/
		class SocketThread
		{
		public:
			/// @name Constructors and Destructor
			/// @{

			/**
			@brief 디폴트 생성자

			@since 1.0.0
			*/
			SocketThread();

			/**
			@brief 디폴트 소멸자

			@since 1.0.0
			*/
			~SocketThread();

			/// @{

		public:

			/**
			@brief 백그라운드 로직을 멈추기 위해 내부 데이터를 초기화한다.

			@since 1.0.0
			*/
			void release();

			/**
			@brief 백그라운드로 처리할 소켓을 추가한다.

			@param[in] key 소켓을 구분할 key
			@param[in] socket 백그라운드로 처리할 소켓

			@since 1.0.0
			*/
			void add(int key, std::shared_ptr<Socket> socket);

			/**
			@brief 백그라운드로 처리중인 소켓을 제거한다.

			@param[in] key 소켓을 구분할 key

			@since 1.0.0
			*/
			void remove(int key);

		public:
			/**
			@brief SocketThread의 instance(Singleton)

			@return SocketThread의 instance

			@since 1.0.0
			*/
			static SocketThread& instance();

		private:
			class Implementation;
			std::unique_ptr<Implementation> impl;

		private:
			static std::unique_ptr<SocketThread> instance_;
		};
	}
}
