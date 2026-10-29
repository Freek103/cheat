#define BF6_KERNEL_BUILD
#include "../common/protocol.h"
#include <wdmsec.h>
static volatile LONG g_events;
static volatile LONG g_dropped;
static PDEVICE_OBJECT g_device;
static const GUID g_dev_guid = {
    0x8c47686f, 0x402c, 0x4f8f,
    { 0x88, 0x02, 0x39, 0xf2, 0xc1, 0x50, 0x20, 0x31 }
};
static NTSTATUS BfDone(PIRP irp, NTSTATUS status, ULONG_PTR info)
{
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = info;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}
static NTSTATUS BfOpenClose(PDEVICE_OBJECT dev, PIRP irp)
{
    UNREFERENCED_PARAMETER(dev);
    return BfDone(irp, STATUS_SUCCESS, 0);
}
NTSTATUS BfIoctl(PDEVICE_OBJECT dev, PIRP irp)
{
    UNREFERENCED_PARAMETER(dev);
    PIO_STACK_LOCATION sp = IoGetCurrentIrpStackLocation(irp);
    ULONG code = sp->Parameters.DeviceIoControl.IoControlCode;
    ULONG in_len = sp->Parameters.DeviceIoControl.InputBufferLength;
    ULONG out_len = sp->Parameters.DeviceIoControl.OutputBufferLength;
    void* buf = irp->AssociatedIrp.SystemBuffer;
    switch (code) {
    case IOCTL_BF6_STATUS: {
        if (out_len < sizeof(BF6_STATUS) || !buf)
            return BfDone(irp, STATUS_BUFFER_TOO_SMALL, 0);
        BF6_STATUS* st = (BF6_STATUS*)buf;
        st->ver = BF6_PROTO;
        st->flags = 0;
        st->dropped = (ULONG)InterlockedCompareExchange(&g_dropped, 0, 0);
        st->events = (ULONG)InterlockedCompareExchange(&g_events, 0, 0);
        return BfDone(irp, STATUS_SUCCESS, sizeof(*st));
    }
    case IOCTL_BF6_EVENT: {
        if (in_len < sizeof(BF6_EVENT) || !buf) {
            InterlockedIncrement(&g_dropped);
            return BfDone(irp, STATUS_INVALID_PARAMETER, 0);
        }
        const BF6_EVENT* ev = (const BF6_EVENT*)buf;
        if (ev->ver != BF6_PROTO ||
            ev->type < BF6_EV_PING || ev->type > BF6_EV_GUARD) {
            InterlockedIncrement(&g_dropped);
            return BfDone(irp, STATUS_INVALID_PARAMETER, 0);
        }
        InterlockedIncrement(&g_events);
        return BfDone(irp, STATUS_SUCCESS, 0);
    }
    default:
        return BfDone(irp, STATUS_INVALID_DEVICE_REQUEST, 0);
    }
}
void BfUnload(PDRIVER_OBJECT drv)
{
    UNICODE_STRING link = RTL_CONSTANT_STRING(L"\\DosDevices\\Bf6Lab");
    IoDeleteSymbolicLink(&link);
    if (g_device)
        IoDeleteDevice(g_device);
    UNREFERENCED_PARAMETER(drv);
}
NTSTATUS DriverEntry(PDRIVER_OBJECT drv, PUNICODE_STRING path)
{
    UNICODE_STRING name = RTL_CONSTANT_STRING(L"\\Device\\Bf6Lab");
    UNICODE_STRING link = RTL_CONSTANT_STRING(L"\\DosDevices\\Bf6Lab");
    UNICODE_STRING acl;
    NTSTATUS status;
    UNREFERENCED_PARAMETER(path);
    RtlInitUnicodeString(&acl, SDDL_DEVOBJ_SYS_ALL_ADM_ALL);
    status = IoCreateDeviceSecure(drv, 0, &name,
                                  FILE_DEVICE_UNKNOWN,
                                  FILE_DEVICE_SECURE_OPEN, FALSE,
                                  &acl, &g_dev_guid, &g_device);
    if (!NT_SUCCESS(status))
        return status;
    drv->MajorFunction[IRP_MJ_CREATE] = BfOpenClose;
    drv->MajorFunction[IRP_MJ_CLOSE] = BfOpenClose;
    drv->MajorFunction[IRP_MJ_DEVICE_CONTROL] = BfIoctl;
    drv->DriverUnload = BfUnload;
    status = IoCreateSymbolicLink(&link, &name);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_device);
        g_device = NULL;
        return status;
    }
    g_device->Flags &= ~DO_DEVICE_INITIALIZING;
    return STATUS_SUCCESS;
}
