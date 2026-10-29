#include "ipc_client.hpp"
KernelClient::KernelClient()
{
    h_ = CreateFileW(L"\\\\.\\Bf6Lab", GENERIC_READ | GENERIC_WRITE,
                     0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}
KernelClient::~KernelClient()
{
    if (h_ != INVALID_HANDLE_VALUE)
        CloseHandle(h_);
}
bool KernelClient::ready() const
{
    return h_ != INVALID_HANDLE_VALUE;
}
bool KernelClient::send(const BF6_EVENT& ev) const
{
    DWORD n = 0;
    return ready() &&
        DeviceIoControl(h_, IOCTL_BF6_EVENT,
                        const_cast<BF6_EVENT*>(&ev), sizeof(ev),
                        nullptr, 0, &n, nullptr) != FALSE;
}
bool KernelClient::status(BF6_STATUS& st) const
{
    DWORD n = 0;
    return ready() &&
        DeviceIoControl(h_, IOCTL_BF6_STATUS,
                        nullptr, 0, &st, sizeof(st),
                        &n, nullptr) != FALSE && n == sizeof(st);
}
bool EventSink::emit(const BF6_EVENT& ev)
{
    return cli_.send(ev);
}
