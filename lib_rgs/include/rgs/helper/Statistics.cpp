#include "stdafx.h"
#include "Statistics.h"

#include "glog/logging.h"

using namespace rgs;

bool Statistics::initialize(const std::wstring& appName)
{
	appName_ = appName;
	if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
	{
		return false;
	}

	return true;
}

unsigned long DiffFileTime(FILETIME * psttOld, FILETIME * psttNew)
{
	unsigned long iTime;

	if (psttNew->dwHighDateTime > psttOld->dwHighDateTime)
	{
		// dwHighDateTime 가 1 증가한 경우만 처리한다.
		iTime = ULONG_MAX - psttOld->dwLowDateTime + psttNew->dwLowDateTime;
	}
	else
	{
		iTime = psttNew->dwLowDateTime - psttOld->dwLowDateTime;
	}

	return iTime;
}

bool Statistics::cpuUsage(double & dbValue)
{
	FILETIME sttKernelTime;
	FILETIME sttUserTime;
	FILETIME sttIdleTime;

	if (!GetSystemTimes(&sttIdleTime, &sttKernelTime, &sttUserTime))
	{
		return false;
	}

	unsigned long iTotalUseTime;
	unsigned long iIdleTime;

	iTotalUseTime = DiffFileTime(&kernelTime, &sttKernelTime);
	iTotalUseTime += DiffFileTime(&userTime, &sttUserTime);

	iIdleTime = DiffFileTime(&idleTime, &sttIdleTime);

	dbValue = (iTotalUseTime - iIdleTime) * 100.0 / iTotalUseTime;

	kernelTime = sttKernelTime;
	userTime = sttUserTime;
	idleTime = sttIdleTime;

	return true;
}

bool SetPrivilege(HANDLE hToken, LPCWSTR pszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES sttTP = { 0 };
	LUID luid;

	if (!LookupPrivilegeValue(NULL, pszPrivilege, &luid))
	{
		LOG(ERROR) << "LookupPrivilegeValue error" << GetLastError();
		return false;
	}

	sttTP.PrivilegeCount = 1;
	sttTP.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
	{
		sttTP.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		sttTP.Privileges[0].Attributes = 0;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &sttTP, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		LOG(ERROR) << "AdjustTokenPrivileges error" << GetLastError();
		return false;
	}

	return true;
}

bool Statistics::memoryUsage(int& workingSet, int& privateUsage)
{
	HANDLE hToken;
	HANDLE hSnap;
	PROCESSENTRY32 sttPE;

	// SeDebugPrivilege 를 설정한다.
	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			if (!ImpersonateSelf(SecurityImpersonation))
			{
				LOG(ERROR) << "ImpersonateSelf error(" << GetLastError() << ") #1";
				return false;
			}

			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
			{
				LOG(ERROR) << "OpenThreadToken error(" << GetLastError() << ") #2";
				return false;
			}
		}
		else
		{
			LOG(ERROR) << "OpenThreadToken error(" << GetLastError() << ") #1";
			return false;
		}
	}

	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
	{
		return false;
	}

	// 프로세스 리스트를 가져온 후, 프로세스 별 메모리를 출력한다.
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		LOG(ERROR) << "CreateToolhelp32Snapshot error";
		return false;
	}

	sttPE.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &sttPE))
	{
		LOG(ERROR) << "Process32First error";
	}
	else
	{
		do
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, sttPE.th32ProcessID);
			if (hProcess)
			{
				std::wstring processName = sttPE.szExeFile;
				
				rgs::convert::toLower(processName);
				rgs::convert::toLower(appName_);

				if (appName_ == processName)
				{
					PROCESS_MEMORY_COUNTERS_EX sttMem;
					sttMem.cb = sizeof(sttMem);

					if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS *)&sttMem, sizeof(sttMem)))
					{
						workingSet = sttMem.WorkingSetSize / 1024;
						privateUsage = sttMem.PrivateUsage / 1024;
					}
					else
					{
						LOG(ERROR) << L"GetProcessMemoryInfo error(" << GetLastError() << ")";
					}
				}
				CloseHandle(hProcess);
			}

		} while (Process32Next(hSnap, &sttPE));
	}


	CloseHandle(hSnap);

	SetPrivilege(hToken, SE_DEBUG_NAME, FALSE);

	CloseHandle(hToken);

	return true;
}