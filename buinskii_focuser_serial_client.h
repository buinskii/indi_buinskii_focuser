
#ifndef BAAFOCUSSERCLI_H
#define BAAFOCUSSERCLI_H

class BuinskiiFocuserSerialClient
{
    protected:
    private:

        int serialportFD = {-1};

        int writeSerial(const char *buffer, int numberOfBytes);
        int readSerial(char *buffer);
        int request(std::string command, std::string *response);
        const uint8_t READ_TIMEOUT = 3;

    public:
        BuinskiiFocuserSerialClient();
        ~BuinskiiFocuserSerialClient();

        void usePort(int port);
		int handshake();
		int setSpeed(int speed);
        int getSpeed(int *currentSpeed);
		int setAcceleration(int acceleration);
        int getAcceleration(int *currentAcceleration);
		int setPosition(int position);
		int setRevers(int revers);
		int moveTo(int position);
		int halt(int *currentPosition);
		int isMoving(int *isMoving);
		int getPosition(int *currentPosition);
};

#endif
