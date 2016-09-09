#include "stdafx.h"
#include "Common.h"
#include "helper/MiniDump.h"
#include "helper/Logger.h"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/coded_stream.h>

#include "boost/algorithm/string.hpp"

#include "io/Socket.h"

using namespace rgs;

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)

boost::shared_ptr<rgs::logger::log_async_sink_t> sink; //Logger.h�� �ʹ� ���ſ��� Common.h�� ���� ���� ����.

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

UpDown::UpDown(const std::string& appName, const std::string& version, int log_level, bool console_log)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

	CMiniDump::Begin();
	sink = rgs::logger::init_async_logger(appName, version, console_log);
	
	boost::log::core::get()->set_filter(severity >= (severity_level)log_level);

	std::wcout.imbue(std::locale("kor"));

	WSADATA	wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	initializeConnectEx();
}

UpDown::~UpDown()
{
	google::protobuf::ShutdownProtobufLibrary();
	WSACleanup();
	
	rgs::logger::stop_logger(sink);

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
	boost::algorithm::to_lower(str);
}

void convert::toLower(std::wstring& str)
{
	boost::algorithm::to_lower(str);
}