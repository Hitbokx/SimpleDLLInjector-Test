#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

DWORD getProcId( const char* procName )
{
    DWORD procId{ 0 };
    HANDLE hSnap{ CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, procId ) };

    if ( hSnap != INVALID_HANDLE_VALUE )
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof( procEntry );

        if ( Process32First( hSnap, &procEntry ) )
        {
            do
            {
                if ( !_stricmp( procEntry.szExeFile, procName ) )
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while ( Process32Next( hSnap, &procEntry ) );
        }
    }
    CloseHandle( hSnap );
    return procId;
}

int main()
{
    const char* dllPath{ "E:\\DEV\\VS Projects\\dlltrainer\\Debug\\dlltrainer.dll" };
    const char* procName{ "ac_client.exe" };
    DWORD procId{ 0 };

    while ( !procId )
    {
        procId = getProcId( procName );
        Sleep( 30 );
    }

    HANDLE hProc{ OpenProcess( PROCESS_ALL_ACCESS,FALSE,procId ) };

    if ( hProc && hProc != INVALID_HANDLE_VALUE )
    {
        void* loc{ VirtualAllocEx( hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ) };
        
        WriteProcessMemory( hProc, loc, dllPath, strlen( dllPath ) + 1, nullptr );

        HANDLE hThread{ CreateRemoteThread( hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, loc, 0, 0 ) };

        if ( hThread )
        {
            CloseHandle( hThread );
        }
    }

    if ( hProc )
    {
        CloseHandle( hProc );
    }
    return 0;
}