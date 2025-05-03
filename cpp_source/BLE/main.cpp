#include <iostream>
#include "BTHConnection.h"
#include "BTHServer.h"

int main()
{
	/*BTHConnection bthConnection;
	bool initialized, isCreateSocket, isConnect, isDataSent, isSuspendForReceiveData, isDataReceived;
	initialized = bthConnection.InitializeBT();

	if (initialized) {
		std::cout << "BLE init" << std::endl;

		isCreateSocket = bthConnection.CreateSocket();

		if (isCreateSocket) {
			std::cout << "BLE socket created" << std::endl;

			isConnect = bthConnection.ConnectToServer("90:78:B2:CD:CC:E0", "aeb9f938-a1a3-4947-ace2-9ebd0c67adf1");

			if (isConnect) {

				std::cout << "BLE Connected" << std::endl;

				isDataSent = bthConnection.SendData("hi dude! I m client");
				isDataSent = bthConnection.SendData("this is test again");

				if (isDataSent) {

					std::cout << "Data sent" << std::endl;

					isSuspendForReceiveData = bthConnection.SuspendSendForReceiveData();

					if (isSuspendForReceiveData) {

						std::cout << "Suspended for receiving data" << std::endl;

						isDataReceived = bthConnection.ReceiveData();

						if (isDataReceived) {

							std::cout << "received data" << std::endl;

						}
						else {

							std::cout << "receiving data failed" << std::endl;

						}
					}
					else {

						std::cout << "Suspended for receiving data failed" << std::endl;

					}
				}
				else {

					std::cout << "Data sent failed" << std::endl;

				}
			}
			else {

				std::cout << "BLE connection failed" << std::endl;

			}
		}
		else {

			std::cout << "BLE socket creation failed" << std::endl;

		}
	}
	else {

		std::cout << "BLE failed" << std::endl;

	}*/


	BTHServer server;

	GUID myServiceUUID = { 0x00001101, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };

	server.SetDataCallback([](const std::string& data) {
		std::cout << "Received: " << data << std::endl;
	});

	server.SetClientConnectedCallback([&]() {
		std::cout << "Client connected!" << std::endl;

		server.SendData("Welcome to AppConnect");
	});

	server.SetClientDisconnectedCallback([]() {
		std::cout << "Client disconnected!" << std::endl;
	});

	server.InitBTHServer();
	if (server.Start("MyBTHServer", myServiceUUID)) {
		std::cout << "Bluetooth SPP server started." << std::endl;

		
	}
	else {
		std::cerr << "Failed to start server." << std::endl;
	}

	std::cin.get(); // Wait for input to quit
	server.StopBTHServer();
	return 0;
}