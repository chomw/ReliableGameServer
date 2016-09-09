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
				DEFAULT_DELAY = 10,	//�۽� �����̸� ����ϴ� �ð� ����
				IMMEDIATELY_SEND = 2				//�۽� ������ ���� �ٷ� �۽��� Ƚ��
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
			static LPFN_CONNECTEX					ConnectEx;			//ConnectEx�� �Լ� ������

		private:
			mutable rgs::thread::sync::SimpleLock	writeLock_;			//write(), sendBuffer() lock
			mutable rgs::thread::sync::SimpleLock	connectionLock_;	//disconnect(), complete() lock

			rgs::io::proactor::Proactor*			proactor_;			//proactors
			rgs::io::proactor::Act acts_[ACT_TYPE_CNT];

			std::atomic<::SOCKET>					rowSocket_;			//winsock row socket
			std::atomic<bool>						isConnect_;			//���� ����
			std::atomic<bool>						isRegisterToIocp_;	//iocp ��� ����
			std::atomic<bool>						isCanceled_;		//���� �Ϸ� ���� ����ߴ��� ����
			std::atomic<bool>						useKeepAlive_;		//keep alive ��뿩��
			std::atomic<bool>						useDelay_;		//keep alive ��뿩��

			unsigned int delayTime_;		//��Ŷ ��ġ�� ���� ���� ���� �ð�
			unsigned int writeCount_;		//���� ��û Ƚ��(write()Ƚ��O, send()Ƚ��X)
			unsigned int receiveSize_;		//������ byte stream ũ��
			unsigned int sendSize_;			//��ģ ��Ŷ ũ��

			DWORD previousSendTime_;					//������ send() �ð�
			DWORD lastCheckTime_;						//������ delayTime_ ��� �ð�
			std::atomic<DWORD> delay_;					//������ų �ð�

			std::atomic<DWORD> keepAliveInterval_;		//keepAlive �ð� ����
			std::atomic<DWORD> keepAliveTimestamp_;		//������ io �ð�

			std::atomic<Socket::State> currentState_;	//���� ����(disconnect)�� ���ȴ�.
			std::atomic<Socket::State> operatingState_;//���� ���� ����

			BYTE receiveBuffer_[rgs::SocketConfig::BUFFER_SIZE];	//���� byte stream ����
			BYTE sendBuffer_[rgs::SocketConfig::BUFFER_SIZE];	//�۽� byte stream ����

			std::shared_ptr<ListenSocket> listenSocket_;		//listen ����
			Session& session_;					//����
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
