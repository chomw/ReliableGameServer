#include "stdafx.h"
#include "Common.h"
#include "helper/MiniDump.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/coded_stream.h>

#include "io/Socket.h"
#include "glog/logging.h"

using namespace rgs;

bool initializeConnectEx()
{
	SOCKET sock;
	DWORD dwBytes;
	int rc;

	/* Dummy socket needed for WSAIoctl */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) return false;

	{
		GUID guid = WSAID_CONNECTEX;
		rc = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid, sizeof(guid),
			&rgs::io::Socket::ConnectEx,
			sizeof(rgs::io::Socket::ConnectEx),
			&dwBytes, NULL, NULL);

		if (rc != 0)
			return false;
	}

	rc = ::closesocket(sock);
	if (rc != 0)
		return false;

	return true;
}

UpDown::UpDown(const std::string& logPath)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

	google::SetLogDestination(google::GLOG_INFO, logPath.c_str());

	google::InitGoogleLogging("");

	CMiniDump::Begin();
	
	std::wcout.imbue(std::locale("kor"));

	WSADATA	wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	initializeConnectEx();
}

UpDown::~UpDown()
{
	WSACleanup();

	google::protobuf::ShutdownProtobufLibrary();
	google::ShutdownGoogleLogging();

	CMiniDump::End();
}

rgs::Delay::Delay(DWORD delay) : delay_(delay)
{
	if (delay_ > 0)
	{
		//���۽ð��� �����Ѵ�.
		if (delay_ <= Second::SECOND_MILLI)
		{
			detailedStartTime_ = std::chrono::system_clock::now();
		}
		else
		{
			startTime_ = ::timeGetTime();
		}
	}
}
rgs::Delay::~Delay()
{
	if (delay_ > 0)
	{
		if (delay_ <= Second::SECOND_MILLI)
		{
			//����ð��� ���Ѵ�.
			std::chrono::nanoseconds nanoElapsedTime = std::chrono::system_clock::now() - detailedStartTime_;
			DWORD milliElapsedTime = (DWORD)nanoElapsedTime.count();

			//�����ð��� ����ð��� �����ϱ� ������ ����ð��� �����ð����� Ŀ���� ������ų �ʿ䰡 ����.
			if (milliElapsedTime < delay_ * SECOND_MILLI)
			{
				DWORD delay = (delay_ * SECOND_MILLI) - milliElapsedTime;
				std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
			}
		}
		else
		{
			//����ð��� ���Ѵ�.
			DWORD elapsedTime = ::timeGetTime() - startTime_;

			//�����ð��� ����ð��� �����ϱ� ������ ����ð��� �����ð����� Ŀ���� ������ų �ʿ䰡 ����.
			if (elapsedTime < delay_ / SECOND_MILLI)
			{
				DWORD delay = (delay_ / SECOND_MILLI) - elapsedTime;
				::Sleep(delay);
			}
		}
	}
}


void convert::toLower(std::string& str)
{
}

void convert::toLower(std::wstring& str)
{
}