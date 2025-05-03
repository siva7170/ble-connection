#pragma once
#include <napi.h>
#include <thread>
#include "BLE/BTHConnection.h"
#include "BLE/BTHServer.h"


class BLEWrapper : public Napi::ObjectWrap<BLEWrapper> {

public:
    BLEWrapper(const Napi::CallbackInfo& info);
    static Napi::Object Init(Napi::Env env, Napi::Object exports);


private:
    static Napi::FunctionReference constructor;
    static Napi::FunctionReference constructorServer;
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

    void BLEWrapper::initBtServer(const Napi::CallbackInfo &info);
    Napi::Value StartServer(const Napi::CallbackInfo& info);
    Napi::Value StopServer(const Napi::CallbackInfo& info);
    Napi::Value SendDataFromServer(const Napi::CallbackInfo& info);
    Napi::Value OnData(const Napi::CallbackInfo& info);
    Napi::Value SetClientConnectedCallback(const Napi::CallbackInfo& info);
    Napi::Value SetClientDisconnectedCallback(const Napi::CallbackInfo& info);

    std::mutex mutex_;
    BTHConnection* bthConnection;
    Napi::ThreadSafeFunction tsfcbRecvData;
    Napi::ThreadSafeFunction tsfcbRecvData2;
    Napi::ThreadSafeFunction tsfcbStatus;
    std::string bluetoothAddr;
    std::string bluetoothUuid;

    // JavaScript callbacks
    Napi::FunctionReference onDataCallback_;
    Napi::FunctionReference clientConnectedCallback_;
    Napi::FunctionReference clientDisconnectedCallback_;

    Napi::ThreadSafeFunction tsfcbClientConnectedCallback_;
    Napi::ThreadSafeFunction tsfcbClientDisconnectedCallback_;
    Napi::ThreadSafeFunction tsfcbDataCallback_;
    
    BTHServer* bthServer;
};