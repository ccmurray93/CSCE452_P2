#include "stdafx.h"

PaintArm paintArm;

Point getJointPosition(int index) {
	Matrix *joint = paintArm.get_T_Matrix(0, index);
	int x = joint->get_elem(0, 3);
	int y = joint->get_elem(1, 3);
	Point p(x, y);
	return p;
}


std::string processJoint(std::string data) {
	std::cout << "Processing Joint (" << data << ")" << std::endl;
	std::string response = "1";

	int joint = data[0] - '0';
	char sign = data[1];

	int r = (sign == '+') ? 1 : -1;


	double t = paintArm.get_T_Matrix(0, 0)->get_elem(0, 3);
	double r1 = paintArm.deg1;
	double r2 = paintArm.deg2;
	paintArm = (*new PaintArm());

	switch (joint) {
		case 0:
			t += r;
			break;
		case 1:
			r1 += r;
			break;
		case 2:
			r2 += r;
			break;
	}

	paintArm.translate(0, t, 0);
	paintArm.rotate(1, r1);
	paintArm.rotate(2, r2);

	return response;
}

std::string processWorld(std::string data) {
	std::string response = "1";

	char axis = data[0];
	char sign = data[1];

	int r = (sign == '+') ? 1 : -1;

	Point endEffector = getJointPosition(3);
	//	Matrix* matrixT = paintArm.get_T_Matrix(0, 3);
	//	double joint_x = matrixT->get_elem(0, 3);
	//	double joint_y = matrixT->get_elem(1, 3);
	//	std::cout << "changeEndEffector: Joint 3 at (" << joint_x << ", " << joint_y << ")" << std::endl;
	//

	int dx = 0;
	int dy = 0;

	if (axis == 'x') dx += r;
	else if (axis == 'y') dy += r;

	double endPosX = endEffector.x + dx;
	double endPosY = endEffector.y + dy;
	int invkin = paintArm.calc_Inverse_Kinematics(endPosX, endPosY);
	double deg1 = paintArm.deg1;
	double deg2 = paintArm.deg2;


	//paintArm = (*new PaintArm());
	//paintArm.rotate(1, deg1);
	//paintArm.rotate(2, deg2);


	if (invkin == 0) {
		//it is reachable!
		response = '1';
	} else if (invkin == 1) {
		//error
		//printf("Error: point not reachable:\n%d, %d\n", endPosX, endPosY);
		response = '0';
	} else {
		//printf("Unknown Error: %i\n", invkin);
		response = '0';
	}

	return response;
}

std::string processReset(std::string data) {
	std::string response = "1";
	paintArm = (*new PaintArm());
	return response;
}

std::string processGet(std::string data) {
	std::cout << "Processing Get (" << data << ")" << std::endl;
	std::string response = "1";
	Point p = getJointPosition(atoi(data.c_str()));
	response = std::to_string(p.x) + "," + std::to_string(p.y);
	return response;
}

std::string processReceived(std::string data) {
	std::cout << '\t' << "Received: " << data << std::endl;
	std::string response = "1";
	char t = data[0];
	switch (t) {
		case 'j':
			// joint control
			response = processJoint(data.substr(1));
			break;
		case 'w':
			// world control
			response = processWorld(data.substr(1));
			break;
		case 'r':
			// reset
			response = processReset(data.substr(1));
			break;
		case 'g':
			// get data
			response = processGet(data.substr(1));
			break;
	}
	return response;
}

























int __cdecl main(void)
{

	paintArm = (*new PaintArm());


	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			std::string temp = "";
			for (int i = 0; i < iResult; ++i) {
				//std::cout << temp << std::endl;
				temp += recvbuf[i];
			}
			//std::cout << '\t' << "Received: " << temp << std::endl;
			std::string response = processReceived(temp);
			std::cout << "Responding with: " << response << std::endl;

			//char *sendbuf = response.c_str();
			iSendResult = send(ClientSocket, response.c_str(), (int)strlen(response.c_str()), 0);

			// Echo the buffer back to the sender
			//iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else  {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}