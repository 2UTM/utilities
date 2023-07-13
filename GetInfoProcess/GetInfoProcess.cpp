#include <windows.h>
#include <stdio.h>
#include "winternl.h"
#include "Ntstatus.h"

#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004

typedef struct _tagThreadInfo
{
    FILETIME ftCreationTime;
    DWORD dwUnknown1;
    DWORD dwStartAddress;
    DWORD dwOwningPID;
    DWORD dwThreadID;
    DWORD dwCurrentPriority;
    DWORD dwBasePriority;
    DWORD dwContextSwitches;
    DWORD dwThreadState;
    DWORD dwWaitReason;
    DWORD dwUnknown2[5];
} THREADINFO, * PTHREADINFO;

typedef struct _tagProcessInfo
{
    DWORD dwOffset;
    DWORD dwThreadCount;
    DWORD dwUnknown1[6];
    FILETIME ftCreationTime;
    DWORD dwUnknown2[5];
    WCHAR* pszProcessName;
    DWORD dwBasePriority;
    DWORD dwProcessID;
    DWORD dwParentProcessID;
    DWORD dwHandleCount;
    DWORD dwUnknown3;
    DWORD dwUnknown4;
    DWORD dwVirtualBytesPeak;
    DWORD dwVirtualBytes;
    DWORD dwPageFaults;
    DWORD dwWorkingSetPeak; // память - пик рабочего набора
    DWORD dwWorkingSet; // память - рабочий набор
    DWORD dwUnknown5;
    DWORD dwPagedPool; // память - выгружаемый пул
    DWORD dwUnknown6;
    DWORD dwNonPagedPool; // память - невыгружаемый пул
    DWORD dwPageFileBytesPeak;
    DWORD dwPrivateBytes; // память - частный рабочий набор
    DWORD dwPageFileBytes; // память - выделенная память
    DWORD dwUnknown7[4];
    THREADINFO ti[0];
} _PROCESSINFO, * PPROCESSINFO;

long(__stdcall* pNtQuerySystemInformation)(ULONG, PVOID, ULONG, ULONG) = NULL;

int main()
{
    PBYTE pbyInfo = NULL;
    DWORD cInfoSize = 0x2000;
    PPROCESSINFO pProcessInfo = { 0 };
    char szProcessName[MAX_PATH] = { 0 };
    if (!(pbyInfo = (PBYTE)malloc(cInfoSize)))
    {
        printf("Ошибка выделения памяти: %d\n", (GetLastError()));
    }
    else
    {
        pNtQuerySystemInformation = (long(__stdcall*)(ULONG, PVOID, ULONG, ULONG))GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");
    }
    if (!pNtQuerySystemInformation)
    {
        printf("Ошибка получения указателя на функцию NtQuerySystemInformation: %d\n", GetLastError());
    }
    else
    {
        while (true)
        {
            while (pNtQuerySystemInformation(SystemProcessInformation, pbyInfo, cInfoSize, 0) == STATUS_INFO_LENGTH_MISMATCH)
            {
                cInfoSize += cInfoSize;
                pbyInfo = (PBYTE)realloc(pbyInfo, cInfoSize);
            }
            pProcessInfo = (PPROCESSINFO)pbyInfo;
            while (true)
            {
                if (pProcessInfo->dwOffset == 0) // если последний процесс
                {
                    break;
                }
                else
                {
                    // исключаем системные процессы и показываем только test.exe
                    if (pProcessInfo->dwProcessID && wcscmp(pProcessInfo->pszProcessName, L"GetInfoProcess.exe") == 0)
                    {
                        wprintf(L"%ls: %03u KB\n", pProcessInfo->pszProcessName, pProcessInfo->dwPageFileBytes / 1024);
                        break;
                    }
                    pProcessInfo = (PPROCESSINFO)((PBYTE)pProcessInfo + pProcessInfo->dwOffset); // следующий процесс
                }
            }
            cInfoSize = cInfoSize;
            pbyInfo = (PBYTE)realloc(pbyInfo, cInfoSize);

            // Исскуственная утечка памяти
            //char* c = new char[5000000];

            Sleep(5000);
        }
    }

    return 0;
}