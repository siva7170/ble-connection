#pragma once
#include <napi.h>
#include <thread>
#include "BLE/BTHConnection.h"


class BLEWrapper : public Napi::ObjectWrap<BLEWrapper> {

public:
    BLEWrapper(const Napi::CallbackInfo& info);
    static Napi::Object Init(Napi::Env env, Napi::Object exports);


private:
    static Napi::FunctionReference constructor;
    void init(const Napi::CallbackInfo& info);
    void connect(const Napi::CallbackInfo& info);
    void onReceiveData(const Napi::CallbackInfo& info);
    void sendData(const Napi::CallbackInfo& info);

    BTHConnection* bthConnection;;
    Napi::ThreadSafeFunction tsfcbRecvData;
};