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
    void setBtInfo(const Napi::CallbackInfo& info);
    void connect(const Napi::CallbackInfo& info);
    void makeConnection(const Napi::CallbackInfo &info);
    void onReceiveData(const Napi::CallbackInfo& info);
    void onReceiveDataFromServer(const Napi::CallbackInfo& info);
    void sendData(const Napi::CallbackInfo& info);
    void sendDataToServer(const Napi::CallbackInfo& info);
    void getLastError2(const Napi::CallbackInfo& info);
    void disconnect(const Napi::CallbackInfo &info);
    void isConnect(const Napi::CallbackInfo &info);
    Napi::Value getStatus(const Napi::CallbackInfo &info);
    void UpdateStatus(const std::string& status);

    BTHConnection* bthConnection;
    Napi::ThreadSafeFunction tsfcbRecvData;
    Napi::ThreadSafeFunction tsfcbRecvData2;
    Napi::ThreadSafeFunction tsfcbStatus;
    std::string bluetoothAddr;
    std::string bluetoothUuid;
};