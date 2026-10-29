#pragma once
#ifdef BF6_KERNEL_BUILD
#include <ntddk.h>
#else
#include <Windows.h>
#include <winioctl.h>
#endif
#define BF6_PROTO 1u
#define BF6_EV_PING 1u
#define BF6_EV_GAME 2u
#define BF6_EV_GUARD 3u
#define IOCTL_BF6_STATUS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_BF6_EVENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_WRITE_DATA)
typedef struct _BF6_STATUS {
    ULONG ver;
    ULONG flags;
    ULONG dropped;
    ULONG events;
} BF6_STATUS;
typedef struct _BF6_EVENT {
    ULONG ver;
    ULONG type;
    ULONG pid;
    ULONG flags;
    ULONGLONG ts;
} BF6_EVENT;
