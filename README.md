# cheat — BF6 offline telemetry lab

Minimal Windows x64 C++/WDK project for a **controlled offline research environment**.

- `Battlefield_6_Offline.exe`: named test-process target for read-only process discovery.
- `Javelin_Lite.exe`: named test-process target for read-only process discovery.
- `bf6_kernel.sys`: WDM driver exposing two bounded IOCTLs (status and test events).
- `bf6_external.exe`: C++20 client that enumerates process names and sends events via `DeviceIoControl`.

No remote injection, memory reading/writing, hooking, TPM spoofing, anti-cheat bypass or networking is included.

## Build

Requirements: Visual Studio C++ (x64), Windows Driver Kit (WDK), CMake 3.20+.

External executable:

```powershell
cmake -S external -B build -A x64
cmake --build build --config Release
```

Driver: open `kernel/bf6_kernel.vcxproj` in Visual Studio with WDK and build for x64. Driver installation and signing must follow the supported Windows requirements for the target test system. The test client needs administrative permissions because of the device ACL.

## IPC

- Windows device: `\\.\Bf6Lab`
- `IOCTL_BF6_STATUS`: returns protocol version, flags and counters.
- `IOCTL_BF6_EVENT`: validates and counts bounded simulation events.

## Validation status

Source files have been checked for presence; Windows/WDK build and driver-load tests were **not** performed in this environment.
