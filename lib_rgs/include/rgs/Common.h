#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "winmm.lib")

#include <stdio.h>
#include <iostream>
#include <process.h>
#include <tchar.h>
#include <locale>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <dbghelp.h>
#include <assert.h>
#include <time.h>
#include <mmsystem.h>
#include <comdef.h>

#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <map>
#include <memory>

#define GLOG_NO_ABBREVIATED_SEVERITIES

namespace rgs
{
	enum class Core { MULTI, SINGLE };

	enum SocketConfig {
		BUFFER_SIZE = 8192,			//버퍼 크기
	};

	enum Error {
		ERROR_ROW_DATA = 1,			//잘못된 RowData 에러
		ERROR_DESERIALIZE = 2,		//deserialize 실패 에러
		ERROR_DISCONNECTED_SESSION = 3, //연결 종료된 세션
		ERROR_NULL_DATA = 4			//널 포인터 데이터로 인한 에러
	};

	class UpDown
	{
	public:
		UpDown(const std::string& logPath);
		virtual ~UpDown();
	};
	
	namespace io
	{
		enum class IoEvent { IO_EVENT_DISCONNECTED, IO_EVENT_CONNECTED, MAX };

		struct IPAddress
		{
			IPAddress() = default;

			IPAddress(const std::string& host, int port) :
				host(host),
				port(port) {}

			std::string host;
			int port;
		};
	}

	inline __int64 GetTick()
	{
		unsigned long dwLow, dwHigh;
		__asm
		{
			rdtsc
				mov	dwLow, eax
				mov	dwHigh, edx
		}
		return (((unsigned __int64)dwHigh << 32) | (unsigned __int64)dwLow) / 1000;
	};

	class Delay
	{
	public:
		enum Second { SECOND = 1000000, SECOND_MILLI = 1000, SECOND_MICRO = 1};

	public:
		explicit Delay(DWORD delay);
		~Delay();
		
	private:
		DWORD delay_;

		DWORD startTime_;
		std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> detailedStartTime_;
	};

	namespace convert
	{
		inline std::wstring s2ws(const std::string& str)
		{
			wchar_t t_str[128] = { 0, };
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str.c_str(), str.size(), t_str, 128);
			std::wstring wstr = t_str;
			return wstr;
		}

		inline bool utf8_to_wstr(const std::string &utf8, std::wstring &wstr)
		{
			std::vector<wchar_t> wbuff(utf8.length() + 1);

			if (!MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.length(), &wbuff[0], utf8.length() + 1))
			{
				return false;
			}

			wstr = &wbuff[0];
			return true;
		}

		inline std::wstring utf8_to_wstr(const std::string& utf8)
		{
			std::wstring wstr;
			utf8_to_wstr(utf8, wstr);
			return wstr;
		}

		inline bool wstr_to_utf8(const std::wstring &wstr, std::string &utf8)
		{
			char utf8Buf[128] = { 0, };

			if (!WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), utf8Buf, 128, NULL, NULL))
			{
				return false;
			}

			utf8 = utf8Buf;

			return true;
		}

		inline std::string wstr_to_utf8(const std::wstring &wstr)
		{
			std::string utf8;
			wstr_to_utf8(wstr, utf8);
			return utf8;
		}

		inline void utf8_to_ascii(const std::string& utf8, std::string& ascii)
		{
			//utf-8 -> wide
			std::wstring wstr;
			utf8_to_wstr(utf8, wstr);
			//wide -> ascii
			_bstr_t b(wstr.c_str());
			ascii = b;
		}

		inline std::string utf8_to_ascii(const std::string& utf8)
		{
			std::string ascii;
			utf8_to_ascii(utf8, ascii);
			return ascii;
		}

		void toLower(std::string& str);
		void toLower(std::wstring& str);
	}

	namespace helper
	{
		namespace tuple
		{
			template<int... Is> struct seq {};
			template<int N, int... Is> struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};
			template<int... Is> struct gen_seq<0, Is...> : seq<Is...>{};

			template<int N, class T, class F>
			inline void apply_one(T& tup, F& func, int index)
			{
				func(std::get<N>(tup), index);
			}

			template<class T, class F, int... Is>
			inline void apply(T& tup, int index, F& func, seq<Is...>)
			{
				static const std::function<void(T&, F&, int)> arr[] = { apply_one<Is, T, F>... };
				if (arr[index])
				{
					arr[index](tup, func, index);
				}
			}

			template<class T, class F>
			inline void apply(T& tup, int index, F& func)
			{
				apply(tup, index, func, gen_seq<std::tuple_size<T>::value>{});
			}
		}
	}
};