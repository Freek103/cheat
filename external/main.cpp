#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>
#include "ipc_client.hpp"
static DWORD find_pid(const wchar_t* name)
{
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (h == INVALID_HANDLE_VALUE)
        return 0;
    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    DWORD pid = 0;
    if (Process32FirstW(h, &pe)) {
        do {
            if (lstrcmpiW(pe.szExeFile, name) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(h, &pe));
    }
    CloseHandle(h);
    return pid;
}
int main()
{
    KernelClient cli;
    if (!cli.ready()) {
        std::fprintf(stderr, "driver unavailable: %lu\n", GetLastError());
        return 1;
    }
    EventSink sink(cli);
    BF6_EVENT ev{};
    ev.ver = BF6_PROTO;
    ev.ts = GetTickCount64();
    ev.pid = GetCurrentProcessId();
    ev.type = BF6_EV_PING;
    if (!sink.emit(ev))
        return 2;
    const DWORD game = find_pid(L"Battlefield_6_Offline.exe");
    const DWORD guard = find_pid(L"Javelin_Lite.exe");
    if (game) {
        ev.pid = game;
        ev.type = BF6_EV_GAME;
        if (!sink.emit(ev))
            return 3;
    }
    if (guard) {
        ev.pid = guard;
        ev.type = BF6_EV_GUARD;
        if (!sink.emit(ev))
            return 4;
    }
    BF6_STATUS st{};
    if (!cli.status(st))
        return 5;
    std::printf("ver=%lu events=%lu dropped=%lu game=%lu guard=%lu\n",
                st.ver, st.events, st.dropped, game, guard);
    return 0;
}
