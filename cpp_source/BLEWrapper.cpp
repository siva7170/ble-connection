#include "BLEWrapper.h"
#include <string>
#include <thread>

using namespace Napi;
Napi::FunctionReference BLEWrapper::constructor;

Napi::Object BLEWrapper::Init(Napi::Env env, Napi::Object exports)
{

    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "BTHConnection", {
        //The names of functions that nodejs calls and what will be invoked on calls.
        InstanceMethod("Initiate", &BLEWrapper::init),
        InstanceMethod("SetBtInfo", &BLEWrapper::setBtInfo),
        InstanceMethod("Connect", &BLEWrapper::connect),
        InstanceMethod("IsConnected", &BLEWrapper::isConnect),
        InstanceMethod("MakeConnection", &BLEWrapper::makeConnection),
        InstanceMethod("OnReceiveData", &BLEWrapper::onReceiveData),
        InstanceMethod("OnReceiveDataFromServer", &BLEWrapper::onReceiveDataFromServer),
        InstanceMethod("SendData", &BLEWrapper::sendData),
        InstanceMethod("SendDataToServer", &BLEWrapper::sendDataToServer),
        InstanceMethod("Disconnect", &BLEWrapper::disconnect),
        InstanceMethod("GetLastError", &BLEWrapper::getLastError2),
        InstanceMethod("GetStatus", &BLEWrapper::getStatus)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("BLEConnection", func); 
   
    return exports;
}

BLEWrapper::BLEWrapper(const Napi::CallbackInfo &info) : Napi::ObjectWrap<BLEWrapper>(info)
{
    this->bthConnection = new BTHConnection(); 
}

void BLEWrapper::init(const Napi::CallbackInfo &info)
{
    try
    {
        if (info.Length() != 2 || !info[0].IsFunction() ||  !info[1].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }
        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[0].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[1].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();
        bool bthInitiated=bthConnection->InitializeBT();
        if(bthInitiated){   
            bool bthSocketCreated=bthConnection->CreateSocket();
            if(bthSocketCreated){   
                successCallback.Call({});
            }
            else{
                failureCallback.Call({});
            } 
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::makeConnection(const Napi::CallbackInfo &info)
{
    
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    try
    {
        if (info.Length() != 2 || !info[0].IsFunction() ||  !info[1].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }

        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[0].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[1].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();

        bool bthConnection1=bthConnection->Connect();
        if(bthConnection1){   
            successCallback.Call({});
            UpdateStatus("CONNECTED");
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::setBtInfo(const Napi::CallbackInfo &info)
{
    try
    {
        if (info.Length() != 4 || !info[0].IsString() ||  !info[1].IsString() ||  !info[2].IsNumber() ||  !info[3].IsNumber())
        {
            Napi::TypeError::New(info.Env(), "Parameter expected").ThrowAsJavaScriptException();
            return;
        }
       
        //this->bluetoothAddr = info[0].As<Napi::String>();
        //this->bluetoothUuid = info[1].As<Napi::String>();

        std::string bluetoothAddr = info[0].As<Napi::String>();
        std::string bluetoothUuid = info[1].As<Napi::String>();
        int timeout = info[2].As<Napi::Number>();
        int noOfAttempt = info[3].As<Napi::Number>();

        bthConnection->SetBthInfo(bluetoothAddr.c_str(),bluetoothUuid.c_str(),timeout,noOfAttempt);

        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::connect(const Napi::CallbackInfo &info)
{
    try
    {
        if (info.Length() != 4 || !info[0].IsString() ||  !info[1].IsString() || !info[2].IsFunction() ||  !info[3].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }
        std::string bluetoothAddr = info[0].As<Napi::String>();
        std::string bluetoothUuid = info[1].As<Napi::String>();
        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[2].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[3].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();
        bool isConnnected=bthConnection->ConnectToServer(bluetoothAddr.c_str(),bluetoothUuid.c_str());
        if(isConnnected){   
            successCallback.Call({});
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

class OnRecvWorker : public Napi::AsyncWorker {
    public:
    OnRecvWorker(Napi::Function& callback,BTHConnection* bthConnection, Napi::ThreadSafeFunction tsfcbRecvData)
    : Napi::AsyncWorker(callback), bthConnection(bthConnection), tsfcbRecvData(tsfcbRecvData) {}

    ~OnRecvWorker() {}

    void Execute() {
        int res;
        int recvbuflen = DEFAULT_BUFLEN;
        char recvbuf[DEFAULT_BUFLEN] = "";
        char testbuf[DEFAULT_BUFLEN] = "";
        do {
            memset(recvbuf, 0, sizeof(recvbuf));
            res = recv(bthConnection->btSocket, recvbuf, recvbuflen, 0);
            if (res > 0) {
               memcpy_s(testbuf,sizeof(testbuf),recvbuf,sizeof(recvbuf));
               afterReceivedData(testbuf);
            }
            else if (res == 0)
                printf("Connection closed\n");
            else
                printf("recv failed: %d\n", WSAGetLastError());
        } while (res > 0);
    }

    void afterReceivedData(const char* msgReceived){
        Napi::Env env = Env();
        std::string strxxx=msgReceived;
        std::string* strtt=&strxxx;
        auto callback = [msgReceived](Napi::Env env, Napi::Function jsCallback,std::string* data) {

            jsCallback.Call({Napi::String::New(env,msgReceived)});
        };
        tsfcbRecvData.BlockingCall(strtt, callback);
        //tsfcbRecvData.Release();
    }


    void OnOK() {
        printf("Received data closed due to connection failure\n");
    }

    private:
        int input;  
        BTHConnection* bthConnection;
        Napi::ThreadSafeFunction tsfcbRecvData;
};

void BLEWrapper::onReceiveData(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() != 1 || !info[0].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }

        Napi::Function napiFunction = info[0].As<Napi::Function>();
        tsfcbRecvData = Napi::ThreadSafeFunction::New(env, napiFunction, "onReceiveDataCallback", 0, 1);

        Napi::Function callback = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {});


        // Create and queue the async worker
        OnRecvWorker* worker = new OnRecvWorker(callback,bthConnection, tsfcbRecvData);
        worker->Queue();
    }
    catch(std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
    
}

class OnRecvWorker2 : public Napi::AsyncWorker {
    public:
    OnRecvWorker2(Napi::Function& callback,BTHConnection* bthConnection, Napi::ThreadSafeFunction tsfcbRecvData2, Napi::ThreadSafeFunction tsfcbStatus)
    : Napi::AsyncWorker(callback), bthConnection(bthConnection), tsfcbRecvData2(tsfcbRecvData2), tsfcbStatus(tsfcbStatus) {}

    ~OnRecvWorker2() {}

    void Execute() {
        int res;
        int recvbuflen = DEFAULT_BUFLEN;
        char recvbuf[DEFAULT_BUFLEN] = "";
        char testbuf[DEFAULT_BUFLEN] = "";
        do {
            memset(recvbuf, 0, sizeof(recvbuf));
            res = recv(bthConnection->btSocket, recvbuf, recvbuflen, 0);
            if (res > 0) {
               memcpy_s(testbuf,sizeof(testbuf),recvbuf,sizeof(recvbuf));
               afterReceivedData(testbuf);
            }
            else if (res == 0){
                printf("Connection closed\n");
                updateStatus("UNEXPECTEDLY_CLOSED_OR_LOST");
                if (bthConnection->Reconnect()) {
                    updateStatus("RECONNECTED");
                    res = 1;
                }
                else{
                    updateStatus("DISCONNECTED");
                }
            }
            else{
                updateStatus("UNEXPECTEDLY_CLOSED_OR_LOST");
                if (bthConnection->Reconnect()) {
                    updateStatus("RECONNECTED");
                    res = 1;
                }
                else{
                    updateStatus("DISCONNECTED");
                }
                printf("recv failed: %d\n", WSAGetLastError());
            }
        } while (res > 0);
    }

    void afterReceivedData(const char* msgReceived){
        Napi::Env env = Env();
        std::string strxxx=msgReceived;
        std::string* strtt=&strxxx;
        auto callback = [msgReceived](Napi::Env env, Napi::Function jsCallback,std::string* data) {

            jsCallback.Call({Napi::String::New(env,msgReceived)});
        };
        tsfcbRecvData2.BlockingCall(strtt, callback);
        //tsfcbRecvData.Release();
    }

    void updateStatus(const std::string& status){
        Napi::Env env = Env();

        if(tsfcbStatus!=nullptr){
            std::string strxxx= std::string(status);
            std::string* strtt=&strxxx;

            napi_status call_status = tsfcbStatus.BlockingCall(strtt,[strxxx](Napi::Env env, Napi::Function jsCallback,std::string* data) {
                jsCallback.Call({Napi::String::New(env, strxxx)});
            });

            // Check if the call to JavaScript succeeded
            if (call_status != napi_ok) {
            // Handle error
            }
        }
    }


    void OnOK() {
        printf("Received data closed due to connection failure\n");
    }

    private:
        int input;  
        BTHConnection* bthConnection;
        Napi::ThreadSafeFunction tsfcbRecvData2;
        Napi::ThreadSafeFunction tsfcbStatus;
};

void BLEWrapper::onReceiveDataFromServer(const Napi::CallbackInfo& info){
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() != 1 || !info[0].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }

        Napi::Function napiFunction = info[0].As<Napi::Function>();
        tsfcbRecvData2 = Napi::ThreadSafeFunction::New(env, napiFunction, "onReceiveDataFromServerCallback", 0, 1);

        Napi::Function callback = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {});


        // Create and queue the async worker
        OnRecvWorker2* worker = new OnRecvWorker2(callback,bthConnection, tsfcbRecvData2, tsfcbStatus);
        worker->Queue();
    }
    catch(std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
    
}

void BLEWrapper::sendData(const Napi::CallbackInfo& info){
 try
    {
        if (info.Length() != 3 || !info[0].IsString() || !info[1].IsFunction() ||  !info[2].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }
        std::string sendMsgStr = info[0].As<Napi::String>();
        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[1].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[2].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();
        bool isSent=bthConnection->SendData(sendMsgStr.c_str());
        if(isSent){   
            successCallback.Call({});
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::sendDataToServer(const Napi::CallbackInfo& info){
 try
    {
        if (info.Length() != 3 || !info[0].IsString() || !info[1].IsFunction() ||  !info[2].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }
        std::string sendMsgStr = info[0].As<Napi::String>();
        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[1].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[2].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();
        bool isSent=bthConnection->SendDataToServer(sendMsgStr.c_str());
        if(isSent){   
            successCallback.Call({});
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::disconnect(const Napi::CallbackInfo& info){
 try
    {
        if (info.Length() != 2 || !info[0].IsFunction() || !info[1].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }
        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[0].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[1].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();
        bool isSent=bthConnection->Disconnect();
        if(isSent){   
            successCallback.Call({});
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::getLastError2(const Napi::CallbackInfo& info){
 try
    {
        if (info.Length() != 0 )
        {
            Napi::TypeError::New(info.Env(), "No param expected").ThrowAsJavaScriptException();
            return;
        }
        int lastError=bthConnection->GetLastError();
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

void BLEWrapper::isConnect(const Napi::CallbackInfo &info)
{
    try
    {
        if (info.Length() != 2 || !info[0].IsFunction() ||  !info[1].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
            return;
        }
        Napi::FunctionReference successCallbackRef = Napi::Persistent(info[0].As<Napi::Function>());
        Napi::FunctionReference failureCallbackRef = Napi::Persistent(info[1].As<Napi::Function>());
        Napi::Function successCallback = successCallbackRef.Value();
        Napi::Function failureCallback = failureCallbackRef.Value();
        bool isConnnected=bthConnection->IsConnected();
        if(isConnnected){   
            successCallback.Call({});
        }
        else{
            failureCallback.Call({});
        } 
        return;
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
        return;
    }
}

Napi::Value BLEWrapper::getStatus(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    try
    {
        if (info.Length() != 1 || !info[0].IsFunction())
        {
            Napi::TypeError::New(info.Env(), "Callback function expected").ThrowAsJavaScriptException();
           return env.Undefined();
        }
        Napi::Function callback = info[0].As<Napi::Function>();

        // Initialize the ThreadSafeFunction with the callback
        tsfcbStatus = Napi::ThreadSafeFunction::New(
            env,
            callback,  // JavaScript function to call
            "getStatusCallback",  // Name
            0,  // Unlimited queue
            1   // Only one thread will use this initially
        );

        return env.Undefined();
    }
    catch (std::exception &e)
    {
        std::string err = "Exception handled in init: " + std::string(e.what());
        //If we want to catch exceptions thrown by cpp side in javascript side, we need to add this line. 
        Napi::Error::New(info.Env(), err).ThrowAsJavaScriptException();
    
        return env.Undefined();
    }
}

void BLEWrapper::UpdateStatus(const std::string& status) {
    // Call the JavaScript function with the status
    
    //   std::string strxxx=msgReceived;
    //     std::string* strtt=&strxxx;
    //     auto callback = [msgReceived](Napi::Env env, Napi::Function jsCallback,std::string* data) {

    //         jsCallback.Call({Napi::String::New(env,msgReceived)});
    //     };

    if(tsfcbStatus!=nullptr){
        std::string strxxx= std::string(status);
        std::string* strtt=&strxxx;

        napi_status call_status = tsfcbStatus.BlockingCall(strtt,[strxxx](Napi::Env env, Napi::Function jsCallback,std::string* data) {
            jsCallback.Call({Napi::String::New(env, strxxx)});
        });

        // Check if the call to JavaScript succeeded
        if (call_status != napi_ok) {
        // Handle error
        }
    }

    
}