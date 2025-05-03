#define UNICODE

#include "BTHServer.h"
#include <iostream>

bool BTHServer::InitBTHServer() {
    //WSADATA wsaData;
    //WSAStartup(MAKEWORD(2, 2), &wsaData);

        // 1. Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    // 2. Initialize Bluetooth
    BLUETOOTH_FIND_RADIO_PARAMS radioParams = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
    HANDLE hRadio = BluetoothFindFirstRadio(&radioParams, &hRadio);
    if (hRadio == NULL) {
        WSACleanup();
        return false;
    }

    // 3. Cleanup and return success
    BluetoothFindRadioClose(hRadio);
    return true;
}

void BTHServer::StopBTHServer() {
    Stop();
    WSACleanup();
}

bool BTHServer::Start(const std::string& serviceName, const GUID& serviceUuid) {
    if (running_) return true;

    // Create RFCOMM socket
    serverSocket_ = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (serverSocket_ == INVALID_SOCKET) return false;

    // Bind socket
    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH;
    sa.port = BT_PORT_ANY;
    if (bind(serverSocket_, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        closesocket(serverSocket_);
        return false;
    }

    // Listen for connections
    if (listen(serverSocket_, 1) == SOCKET_ERROR) {
        closesocket(serverSocket_);
        return false;
    }    
    
    std::wstring wideName = stringToWide(serviceName);
    std::wstring wideComment = L"Bluetooth SPP Server";

    // Get local socket address
    int saLen = sizeof(sa);
    getsockname(serverSocket_, (SOCKADDR*)&sa, &saLen); // Required for correct port info

    // Register service
    CSADDR_INFO csAddr = { 0 };
    csAddr.LocalAddr.iSockaddrLength = sizeof(sa);
    csAddr.LocalAddr.lpSockaddr = (SOCKADDR*)&sa;
    csAddr.iSocketType = SOCK_STREAM;
    csAddr.iProtocol = BTHPROTO_RFCOMM;

    WSAQUERYSET service = { 0 };
    service.dwSize = sizeof(service);
    service.lpszServiceInstanceName = const_cast<LPWSTR>(wideName.c_str());
    service.lpszComment = const_cast<LPWSTR>(wideComment.c_str());
    service.lpServiceClassId = const_cast<GUID*>(&serviceUuid); // REQUIRED
    service.dwNameSpace = NS_BTH;
    service.dwNumberOfCsAddrs = 1;
    service.lpcsaBuffer = &csAddr;

    /*BLOB blob = { 0 };
    blob.cbSize = sizeof(serviceUuid);
    blob.pBlobData = (BYTE*)&serviceUuid;
    service.lpBlob = &blob;*/

    if (WSASetService(&service, RNRSERVICE_REGISTER, 0) == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        std::cerr << "WSASetService failed with error: " << errorCode << std::endl;
        closesocket(serverSocket_);
        return false;
    }

    running_ = true;
    acceptThread_ = std::thread(&BTHServer::AcceptThread, this);
    return true;
}

void BTHServer::Stop() {
    running_ = false;

    if (clientSocket_ != INVALID_SOCKET) {
        shutdown(clientSocket_, SD_BOTH);
        closesocket(clientSocket_);
        clientSocket_ = INVALID_SOCKET;
    }

    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }

    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }

    if (clientThread_.joinable()) {
        clientThread_.join();
    }
}

//void BTHServer::AcceptThread() {
//    while (running_) {
//        SOCKADDR_BTH clientAddr = { 0 };
//        int addrLen = sizeof(clientAddr);
//
//        SOCKET clientSocket = accept(serverSocket_, (SOCKADDR*)&clientAddr, &addrLen);
//        if (clientSocket == INVALID_SOCKET) continue;
//
//        if (clientSocket_ != INVALID_SOCKET) {
//            closesocket(clientSocket);
//            continue;
//        }
//
//        clientSocket_ = clientSocket;
//
//        if (onClientConnected_) {
//            onClientConnected_();
//        }
//
//        clientThread_ = std::thread(&BTHServer::ClientHandler, this, clientSocket_);
//    }
//}

void BTHServer::AcceptThread() {
    while (running_) {
        SOCKADDR_BTH clientAddr = { 0 };
        int addrLen = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket_, (SOCKADDR*)&clientAddr, &addrLen);
        if (clientSocket == INVALID_SOCKET) continue;

        std::cout << "Client attempting to connect..." << std::endl;

        // Make sure previous client thread is finished
        if (clientThread_.joinable()) {
            clientThread_.join();
        }

        if (clientSocket_ != INVALID_SOCKET) {
            closesocket(clientSocket);
            std::cout << "Rejected client: already connected." << std::endl;
            continue;
        }

        std::cout << "Client connected." << std::endl;
        clientSocket_ = clientSocket;
        if (onClientConnected_) {
            onClientConnected_();  // <-- Trigger callback
        }
        clientThread_ = std::thread(&BTHServer::ClientHandler, this, clientSocket_);
    }
}


//void BTHServer::ClientHandler(SOCKET clientSocket) {
//    char buffer[1024];
//    while (running_) {
//        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
//        if (bytesReceived <= 0) break;
//
//        if (dataCallback_) {
//            dataCallback_(std::string(buffer, bytesReceived));
//        }
//    }
//
//    closesocket(clientSocket);
//    clientSocket_ = INVALID_SOCKET;
//
//    if (onClientDisconnected_) {
//        onClientDisconnected_();
//    }
//}

void BTHServer::ClientHandler(SOCKET clientSocket) {
    char buffer[1024];

    while (running_) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        if (dataCallback_) {
            dataCallback_(std::string(buffer, bytesReceived));
        }
    }

    std::cout << "Client disconnected." << std::endl;
    if (onClientDisconnected_) {
        onClientDisconnected_();  // <-- Trigger callback
    }
    // Clean up
    closesocket(clientSocket);
    clientSocket_ = INVALID_SOCKET;
}


bool BTHServer::SendData(const std::string& data) {
    if (clientSocket_ == INVALID_SOCKET) return false;

    return send(clientSocket_, data.c_str(), (int)data.size(), 0) != SOCKET_ERROR;
}

void BTHServer::SetDataCallback(DataCallback callback) {
    dataCallback_ = callback;
}

void BTHServer::SetClientConnectedCallback(ClientEventCallback callback) {
    onClientConnected_ = callback;
}

void BTHServer::SetClientDisconnectedCallback(ClientEventCallback callback) {
    onClientDisconnected_ = callback;
}

// Convert std::string to std::wstring
std::wstring BTHServer::stringToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
    return wstr;
}