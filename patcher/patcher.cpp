// Main patcher logic, contains all Windows API calls and stuff. Memory addresses are hardcoded replacements as of writing.

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <iostream>

using namespace std;

// Function to return a PID from name
DWORD getPIDByName(const TCHAR* processName) {
    cout << "Creating process snapshot to search for destiny2.exe" << endl;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Handle to all running processes on the system
    if (snapshot == INVALID_HANDLE_VALUE) {
        cerr << "Unable to create process snapshot handle." << endl;
        exit(1);
    }

    // Create blank processentry and set the size for later use
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    // Take the first process from the snapshot and confirm it exists
    if (!Process32First(snapshot, &pe32)) {
        // Assume no process was found
        cerr << "First process from snapshot not found." << endl;
        CloseHandle(snapshot);
        exit(1);
    }

    // Iterate through processes until process with name is found
    do {
        if (_tcscmp(pe32.szExeFile, processName) == 0) {
            CloseHandle(snapshot);
            return pe32.th32ProcessID;
        }
    } while (Process32Next(snapshot, &pe32));

    cerr << "Destiny 2 process not found. Verify the game is running.";
    CloseHandle(snapshot);
    exit(1);
}

int main()
{
    const TCHAR* processName = _T("destiny2.exe");

    DWORD d2PID = getPIDByName(processName);

    cout << "Destiny 2 process found with PID " << d2PID << endl;

    // Wait for user to close the window
    while (true) {}
}

