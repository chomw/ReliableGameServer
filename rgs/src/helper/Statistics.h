#pragma once

#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>

#pragma comment( lib, "psapi" )

namespace rgs
{
	class Statistics
	{
	public:
		bool initialize(const std::wstring& appName);
		bool cpuUsage(double & value);
		bool memoryUsage(int& workingSet, int& privateUsage);

	private:
		std::wstring appName_;
		FILETIME kernelTime;
		FILETIME userTime;
		FILETIME idleTime;
	};
}