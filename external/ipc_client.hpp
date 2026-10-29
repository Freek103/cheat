#pragma once
#include "../common/protocol.h"
class KernelClient {
    HANDLE h_ = INVALID_HANDLE_VALUE;
public:
    KernelClient();
    ~KernelClient();
    KernelClient(const KernelClient&) = delete;
    KernelClient& operator=(const KernelClient&) = delete;
    bool ready() const;
    bool send(const BF6_EVENT& ev) const;
    bool status(BF6_STATUS& st) const;
};
class EventSink {
    KernelClient& cli_;
public:
    explicit EventSink(KernelClient& cli) : cli_(cli) {}
    bool emit(const BF6_EVENT& ev);
};
