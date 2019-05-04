
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdarg.h>
#include <indidevapi.h>

#include "buinskii_focuser_serial_client.h"

BuinskiiFocuserSerialClient::BuinskiiFocuserSerialClient()
{

}

BuinskiiFocuserSerialClient::~BuinskiiFocuserSerialClient()
{

}

void BuinskiiFocuserSerialClient::usePort(int port) {
	serialportFD = port;
}

int BuinskiiFocuserSerialClient::handshake() {
	return request("T", nullptr);
}

int BuinskiiFocuserSerialClient::setSpeed(int speed) {
	std::string cmd = "SS ";
	cmd.append(std::to_string(speed));

	return request(cmd, nullptr);
}

int BuinskiiFocuserSerialClient::getSpeed(int *currentSpeed) {
    std::string *result = new std::string();

    int success = request("GS", result);
    if (!success) {
        delete result;
        return success;
    }

    *currentSpeed = atoi(result->c_str());

    delete result;
    return 1;
}

int BuinskiiFocuserSerialClient::setAcceleration(int acceleration) {
    std::string cmd = "SA ";
    cmd.append(std::to_string(acceleration));

    return request(cmd, nullptr);
}

int BuinskiiFocuserSerialClient::getAcceleration(int *currentAcceleration) {
    std::string *result = new std::string();

    int success = request("GA", result);
    if (!success) {
        delete result;
        return success;
    }

    *currentAcceleration = atoi(result->c_str());

    delete result;
    return 1;
}

int BuinskiiFocuserSerialClient::setRevers(int revers) {
	std::string cmd = "Z ";
	cmd.append(std::to_string(revers));

	return request(cmd, nullptr);
}

int BuinskiiFocuserSerialClient::moveTo(int position) {
	std::string cmd = "M ";
	cmd.append(std::to_string(position));

	return request(cmd, nullptr);
}

int BuinskiiFocuserSerialClient::halt(int *currentPosition) {
	std::string *result = new std::string();

	int success = request("H", result);
	if (!success) {
		delete result;
		return success;
	}

	*currentPosition = atoi(result->c_str());

	delete result;
	return 1;
}

int BuinskiiFocuserSerialClient::isMoving(int *isMoving) {
	*isMoving = 0;
	std::string *result = new std::string();

	int success = request("I", result);
	if (!success) {
		delete result;
		return success;
	}

	*isMoving = atoi(result->c_str());

	delete result;
	return 1;
}

int BuinskiiFocuserSerialClient::setPosition(int position) {
    std::string cmd = "SP ";
    cmd.append(std::to_string(position));

    return request(cmd, nullptr);
}

int BuinskiiFocuserSerialClient::getPosition(int *currentPosition) {
	std::string *result = new std::string();

	int success = request("GP", result);
	if (!success) {
		delete result;
		return success;
	}

	*currentPosition = atoi(result->c_str());

	delete result;
	return 1;
}

void BuinskiiFocuserSerialClient::printMessage(const char *fmt, ...)
{
	if (verbose)
	{
		va_list args;
		va_start(args, fmt);
		char outputBuffer[2000];
		vsprintf(outputBuffer, fmt, args);
		va_end(args);

		std::cout << outputBuffer;
		IDLog(outputBuffer);
	}
}

int BuinskiiFocuserSerialClient::request(std::string command, std::string *result) {
	std::string *response = new std::string();

	std::string fullCommand = command;
	fullCommand.append("\n");

	char *writeBuffer = strdup(fullCommand.c_str());

	printMessage("[request] command=\"%s\"; size=%d\n", command.c_str(), fullCommand.size());
	writeSerial(writeBuffer, fullCommand.size());

    char received[1];
	int receiveResult = 0;
	do {
		receiveResult = readSerial(received, 1);

		if (receiveResult < 0) {
			delete response;
			return receiveResult;
		}
		response->append(received);
		printMessage("[request] now response is =\"%s\" received %s\n", response->c_str(), received);
	} while (strcmp(received, "\n") != 0);

	response->erase(response->find_last_not_of(" \n\r\t#") + 1);

	printMessage("[request] command=\"%s\"; size=%d; response=\"%s\"\n", command.c_str(), fullCommand.size(), response->c_str());

	std::string delimiter = ":";

	size_t pos = 0;
	std::string resultType;
	std::string commandInterpreted;
	std::string commandResult;

	if ((pos = response->find(delimiter)) != std::string::npos) {
		resultType = response->substr(0, pos);
		response->erase(0, pos + delimiter.length());
	}

	if ((pos = response->find(delimiter)) != std::string::npos) {
		commandInterpreted = response->substr(0, pos);
		response->erase(0, pos + delimiter.length());

		if ((pos = response->find(delimiter)) != std::string::npos) {
			commandResult = response->substr(0, pos);
			response->erase(0, pos + delimiter.length());
		}
		else {
			commandResult = response->substr(0, response->size());
		}
	}
	else {
		commandInterpreted = response->substr(0, response->size());
	}

	if (strcmp(resultType.c_str(), "A") != 0) {
		printMessage("[request] failed command=\"%s\"; not ack, but \"%s\"\n", command.c_str(), resultType.c_str());
		delete response;
		return -1;
	}
	if (strcmp(commandInterpreted.c_str(), command.c_str()) != 0) {
		printMessage("[request] failed command=\"%s\"; response command mismatch, given \"%s\"\n", command.c_str(), commandInterpreted.c_str());
		delete response;
		return -1;
	}

	if (result != nullptr) {
		result->append(commandResult);
	}

	printMessage("[request] success command=\"%s\"; result=\"%s\"\n", command.c_str(), commandResult.c_str());

	delete response;
	return 1;
}

int BuinskiiFocuserSerialClient::writeSerial(const char *buffer, int numberOfBytes)
{
	printMessage("writeSerial: writting %d bytes\n", numberOfBytes);

	int n = write(serialportFD, buffer, numberOfBytes);

	if (n < numberOfBytes)
	{
		printMessage("writeSerial: written of %d bytes failed!\n", numberOfBytes);
		return -1;
	}

	printMessage("writeSerial: wroten\n");

	return n;
}

int BuinskiiFocuserSerialClient::readSerial(char *buffer, int numberOfBytes)
{
	int n = 0;
	int a = 0;

	while (n < numberOfBytes)
	{
		int readed = read(serialportFD, &(buffer[n]), numberOfBytes - n);
		if (readed < 0)
		{
			printMessage("readSerial: read of %d bytes failed!\n", numberOfBytes);
			char *errmsg = strerror(errno);
			printMessage("Read error: %d %s\n", errno, errmsg);


			if (a > 10) {
				return (-1);
			}

			a++;
			readed = 0;
			sleep(1);
		}
		else {
            printMessage("readSerial: read of %d bytes success!\n", numberOfBytes);
		}

		n += readed;
	}

	return n;
}
