#pragma once
#include <winsock2.h>
#include <ws2bth.h>
#include <windows.h>
#include <bluetoothapis.h>
#include <functional>
#include <string>
#include <thread>
#pragma comment(lib,"Bthprops")
#pragma comment(lib,"WS2_32")
#pragma comment (lib, "User32.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")

class BTHServer {
public:
    using DataCallback = std::function<void(const std::string&)>;
    using ClientEventCallback = std::function<void()>;

    bool InitBTHServer();
    void StopBTHServer();

    bool Start(const std::string& serviceName, const GUID& serviceUuid);
    void Stop();
    bool SendData(const std::string& data);

    void SetDataCallback(DataCallback callback);
    void SetClientConnectedCallback(ClientEventCallback callback);
    void SetClientDisconnectedCallback(ClientEventCallback callback);

private:
    void AcceptThread();
    void ClientHandler(SOCKET clientSocket);
    std::wstring stringToWide(const std::string& str);

    SOCKET serverSocket_ = INVALID_SOCKET;
    SOCKET clientSocket_ = INVALID_SOCKET;
    std::thread acceptThread_;
    std::thread clientThread_;
    DataCallback dataCallback_;
    ClientEventCallback onClientConnected_;
    ClientEventCallback onClientDisconnected_;
    bool running_ = false;
};