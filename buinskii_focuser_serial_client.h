
#ifndef BAAFOCUSSERCLI_H
#define BAAFOCUSSERCLI_H

class BuinskiiFocuserSerialClient
{
    protected:
    private:

        int verbose = false;
        int serialportFD = {-1};

        void printMessage(const char *fmt, ...);
        int writeSerial(const char *buffer, int numberOfBytes);
        int readSerial(char *buffer, int numberOfBytes);
        int request(std::string command, std::string *response);

    public:
        BuinskiiFocuserSerialClient();
        ~BuinskiiFocuserSerialClient();

        void usePort(int port);
		int handshake();
		int setSpeed(int speed);
        int getSpeed(int *currentSpeed);
		int setAcceleration(int acceleration);
		int setPosition(int position);
		int setRevers(int revers);
		int moveTo(int position);
		int halt(int *currentPosition);
		int isMoving(int *isMoving);
		int getPosition(int *currentPosition);
};

#endif
