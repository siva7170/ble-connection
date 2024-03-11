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
        InstanceMethod("Connect", &BLEWrapper::connect),
        InstanceMethod("OnReceiveData", &BLEWrapper::onReceiveData),
        InstanceMethod("SendData", &BLEWrapper::sendData),
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
        printf("OnOk\n");
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
