// Main patcher logic, contains all Windows API calls and stuff. Memory addresses are hardcoded replacements as of writing.

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <list>

using namespace std;

// Function to return a PID from name
DWORD getPIDByName(const TCHAR* processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Handle to all running processes on the system
    if (snapshot == INVALID_HANDLE_VALUE) {
        cerr << "Unable to create process snapshot handle." << endl;
        return 0;
    }

    // Create blank processentry and set the size for later use
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    // Take the first process from the snapshot and confirm it exists
    if (!Process32First(snapshot, &pe32)) {
        CloseHandle(snapshot);
        return 0;
    }

    // Iterate through processes until process with name is found
    do {
        if (_tcscmp(pe32.szExeFile, processName) == 0) {
            CloseHandle(snapshot);
            return pe32.th32ProcessID;
        }
    } while (Process32Next(snapshot, &pe32));

    // Process not yet running
    CloseHandle(snapshot);
    return 0;
}

// Function to patch D2 memory where needed
bool patch(DWORD d2PID) {
    string newAddress = "127.0.0.1";

    list<LPVOID> targets;

    // Addresses to be patched
    targets.push_back((LPVOID)0x7FF773FDED68);
    targets.push_back((LPVOID)0x7FF773FDED78);
    targets.push_back((LPVOID)0x7FF773FDED88);
    targets.push_back((LPVOID)0x7FF773FDED98);
    targets.push_back((LPVOID)0x7FF774300C35);
    targets.push_back((LPVOID)0x7FF77437DA48);
    targets.push_back((LPVOID)0x7FF773FB4531);
    targets.push_back((LPVOID)0x7FF77413B548);
    targets.push_back((LPVOID)0x7FF773FB4A08);
    targets.push_back((LPVOID)0x7FF773FB49E0);
    targets.push_back((LPVOID)0x7FF77436EE80);
    targets.push_back((LPVOID)0x7FF77417F8C0);
    targets.push_back((LPVOID)0x7FF773FBB670);
    targets.push_back((LPVOID)0x7FF77437DA38);
    targets.push_back((LPVOID)0x7FF77414D198);


    // Open a handle to D2
    HANDLE d2 = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        false, d2PID);
    if(!(d2)){
        cerr << "Failed to open D2 process because " << GetLastError() << endl;
        return false;
    }

    // Write size
    SIZE_T writeSize = newAddress.size() + 1;

    // Wait for Bungie servers to appear in memory addresses
    // IMPLEMENT LATER: FOR NOW JUST SLEEP ON IT

    // PATCHING AT LAUNCH CAUSES TERMINATION: EITHER WAIT OR (SOMEDAY) FIND A WAY TO PATCH OUT INTEGRITY CHECKS

    // Unfortunately, sleeping isn't gonna cut it. We're gonna have to snap the game's integrity checks.

    Sleep(2000);

    // Overwrite Bungie server address with localhost
    list<BOOL> results;

    for (LPVOID address : targets) {
        results.push_back(WriteProcessMemory(d2, address, newAddress.c_str(), writeSize, NULL));
    }

    // Close the handle
    CloseHandle(d2);

    // Return true only if every address succeeded
    for (BOOL result : results) {
        if (!result) {
            cerr << "Failed to patch one or more addresses." << endl;
            return false;
       }
    }

    return true;
}

int main()
{
    // Continously scan for a D2 process, patch it when found
    const TCHAR* processName = _T("destiny2.exe");
    DWORD d2PID = 0;

    cout << "Searching for destiny2.exe..." << endl;

    do {
        d2PID = getPIDByName(processName);
        if (d2PID) { break; }
        Sleep(5);
    } while (true);

    cout << "Destiny 2 process found with PID " << d2PID << endl;

    // Invoke the patch function
    cout << "Attempting to patch D2 server addresses..." << endl;

    if (patch(d2PID))
        cout << "Successfully patched server addresses!" << endl;
    else
        cout << "Failed to patch server addresses." << endl;

    // Wait for user to close the window
    while (true) {
        Sleep(60);
    }
}

