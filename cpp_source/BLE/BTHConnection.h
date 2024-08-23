#pragma once
#include <winsock2.h>
#include <ws2bth.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#pragma comment(lib,"WS2_32")
#pragma comment (lib, "User32.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")

#define DEFAULT_BUFLEN 512


class BTHConnection {
public:
    int error;
    int lastError;
    int timeOutForReconnect;
    int noofAttempt;
    SOCKET btSocket;
    SOCKADDR_BTH sockAddr;
    const char* btaddr;
    const char* btguid;
    std::string connectionStatus;
    bool isDisconnectedRequest;

    bool InitializeBT();
    bool CreateSocket();
    void SetBthInfo(const char* straddr, const char* guid, int timeoutForReconnect, int noOfAttempt);
    bool Connect();
    bool IsConnected();
    bool ConnectToServer(const char* straddr, const char* guid);
    bool SendData(const char* msgToServer);
    bool SendDataToServer(const char* msgToServer);
    bool SuspendSendForReceiveData();
    bool ReceiveData();
    bool ReceiveDataFromServer();
    bool Reconnect();
    bool CloseSocket();
    void FinalizeBT();
    bool Disconnect();
    int GetLastError();

    int str2ba(const char* straddr, BTH_ADDR* btaddr);
    int str2guid(const char* s, GUID* guidObj);
};