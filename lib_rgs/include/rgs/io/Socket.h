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
		
		enum class IoEvent;
		class ListenSocket;
		class Session;
		
		class Socket : public std::enable_shared_from_this<Socket>
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
				DEFAULT_DELAY = 10,	//송신 딜레이를 계산하는 시간 간격
				IMMEDIATELY_SEND = 2				//송신 딜레이 없이 바로 송신할 횟수
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
			Socket(Session&);
			virtual ~Socket();

			//row socket
			bool createRowSocket();

			//IO functions
			bool write(BYTE* data, unsigned int size);
			
			bool startReceive();
			bool receive(unsigned int receiveBytes);


			bool bind();
			bool connect(const std::string& address, USHORT port, BYTE* data = nullptr, unsigned int size = 0);
			bool disconnect();
			bool listening();

			void complete(rgs::io::IoEvent ioEvent);

			bool completeListening();
			void sendDelayed();

		public:
			void setConnected(bool);
			void setProactor(rgs::io::proactor::Proactor* proactor, rgs::io::proactor::Acceptor* acceptor, rgs::io::proactor::Disconnector* disconnector, rgs::io::proactor::Sender* sender, rgs::io::proactor::Receiver* receiver, rgs::io::proactor::Connector* connector);
			void setListenSocket(std::shared_ptr<ListenSocket> listenSocket);

			::SOCKET					getRowSocket() const;
			bool						getRegisterToIocp()const;
			
			bool isConnected()const;
			bool isValidRowSocket()const;

			void useKeepAlive(bool);
			bool isExpired();
			void setKeepAliveInterval(DWORD interval);
			void setKeepAliveTimestamp(DWORD time);

			void setDelay(bool useDelay);
			void setDelay(bool useDelay, DWORD delay);

		private:
			void initialize();

			bool send(BYTE* data, unsigned int size);

			void setRegisterToIocp(bool);
			bool readAddress();

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
			std::atomic<bool>						useKeepAlive_;		//keep alive 사용여부
			std::atomic<bool>						useDelay_;		//keep alive 사용여부

			unsigned int delayTime_;		//패킷 뭉치기 위한 전송 지연 시간
			unsigned int writeCount_;		//전송 요청 횟수(write()횟수O, send()횟수X)
			unsigned int receiveSize_;		//수신한 byte stream 크기
			unsigned int sendSize_;			//뭉친 패킷 크기

			DWORD previousSendTime_;					//마지막 send() 시간
			DWORD lastCheckTime_;						//마지막 delayTime_ 계산 시간
			std::atomic<DWORD> delay_;					//지연시킬 시간

			std::atomic<DWORD> keepAliveInterval_;		//keepAlive 시간 간격
			std::atomic<DWORD> keepAliveTimestamp_;		//마지막 io 시간

			std::atomic<Socket::State> currentState_;	//소켓 상태(disconnect)로 사용된다.
			std::atomic<Socket::State> operatingState_;//소켓 동작 상태

			BYTE receiveBuffer_[rgs::SocketConfig::BUFFER_SIZE];	//수신 byte stream 버퍼
			BYTE sendBuffer_[rgs::SocketConfig::BUFFER_SIZE];	//송신 byte stream 버퍼

			std::shared_ptr<ListenSocket> listenSocket_;		//listen 소켓
			Session& session_;					//세션
		};

		class ListenSocket
		{
		public:
			void initialize(rgs::io::proactor::Proactor* proactor, unsigned int port);
			bool listen(Socket* acceptSocket, char* buffer, unsigned int bufferSize, rgs::io::proactor::Act* act);

			::SOCKET getRowSocket()const;

		private:
			::SOCKET rowSocket_;
		};

		class SocketMonitor
		{
		public:
			SocketMonitor();
			~SocketMonitor();

			void release();

			void add(int key, std::shared_ptr<Socket> socket);

			void remove(int key);

		public:
			static SocketMonitor& instance();

		private:
			class Implementation;
			std::unique_ptr<Implementation> impl;

		private:
			static std::unique_ptr<SocketMonitor> instance_;
		};
	}
}
