#include <iostream>
#include "BTHConnection.h"
int main()
{
	BTHConnection bthConnection;
	bool initialized, isCreateSocket, isConnect, isDataSent, isSuspendForReceiveData, isDataReceived;
	initialized = bthConnection.InitializeBT();

	if (initialized) {
		std::cout << "BLE init" << std::endl;

		isCreateSocket = bthConnection.CreateSocket();

		if (isCreateSocket) {
			std::cout << "BLE socket created" << std::endl;

			isConnect= bthConnection.ConnectToServer("90:78:B2:CD:CC:E0", "aeb9f938-a1a3-4947-ace2-9ebd0c67adf1");

			if (isConnect) {
				std::cout << "BLE Connected" << std::endl;

				isDataSent = bthConnection.SendData("hi dude! I m client");
				isDataSent = bthConnection.SendData("this is test again");

				if (isDataSent) {
					std::cout << "Data sent" << std::endl;

					isSuspendForReceiveData= bthConnection.SuspendSendForReceiveData();

					if (isSuspendForReceiveData) {
						std::cout << "Suspended for receiving data" << std::endl;
						isDataReceived= bthConnection.ReceiveData();

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
	}
}