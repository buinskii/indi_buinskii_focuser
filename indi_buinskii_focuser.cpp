
#include <stdio.h>
#include <memory>
#include <cmath>
#include <cstring>
#include <iostream>
#include <indiapi.h>

#include "indi_buinskii_focuser.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1

#define FOCUSNAMEF "Buinskii Focuser"

std::unique_ptr<IndiBuinskiiFocuser> indiBuinskiiFocuser(new IndiBuinskiiFocuser);

void ISPoll(void *p);

void ISGetProperties(const char *dev) {
    indiBuinskiiFocuser->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num) {
    indiBuinskiiFocuser->ISNewSwitch(dev, name, states, names, num);
}

void ISNewText(const char *dev, const char *name, char *texts[], char *names[], int num) {
    indiBuinskiiFocuser->ISNewText(dev, name, texts, names, num);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num) {
    indiBuinskiiFocuser->ISNewNumber(dev, name, values, names, num);
}

void ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[],
               char *names[], int n) {
    INDI_UNUSED(dev);
    INDI_UNUSED(name);
    INDI_UNUSED(sizes);
    INDI_UNUSED(blobsizes);
    INDI_UNUSED(blobs);
    INDI_UNUSED(formats);
    INDI_UNUSED(names);
    INDI_UNUSED(n);
}

void ISSnoopDevice(XMLEle *root) {
    indiBuinskiiFocuser->ISSnoopDevice(root);
}

IndiBuinskiiFocuser::IndiBuinskiiFocuser() {
    client = new BuinskiiFocuserSerialClient();

    setVersion(MAJOR_VERSION, MINOR_VERSION);
    setSupportedConnections(CONNECTION_SERIAL);
    FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_ABORT | FOCUSER_HAS_VARIABLE_SPEED);
}

IndiBuinskiiFocuser::~IndiBuinskiiFocuser() {
    delete client;
}

const char *IndiBuinskiiFocuser::getDefaultName() {
    return FOCUSNAMEF;
}

bool IndiBuinskiiFocuser::initProperties() {
    INDI::Focuser::initProperties();

    FocusSpeedN[0].min   = 1;
    FocusSpeedN[0].max   = 100;
    FocusSpeedN[0].value = 30;

    FocusAbsPosN[0].min   = 0.;
    FocusAbsPosN[0].max   = 60000.;
    FocusAbsPosN[0].value = 0;
    FocusAbsPosN[0].step  = 200;

    setDefaultPollingPeriod(500);
    addDebugControl();

    return true;
}

bool IndiBuinskiiFocuser::updateProperties() {
    INDI::Focuser::updateProperties();

    if (isConnected())
    {
        client->usePort(PortFD);

        int currentPosition = 0;
        if (client->getPosition(&currentPosition) > 0) {
            FocusAbsPosN->value = currentPosition;
        }

        int currentSpeed = 0;
        if (client->getSpeed(&currentSpeed) > 0) {
            FocusSpeedN->value = currentSpeed;
        }
    }

    return true;
}
bool IndiBuinskiiFocuser::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) {
    return INDI::Focuser::ISNewNumber(dev, name, values, names, n);
}

bool IndiBuinskiiFocuser::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) {
    return INDI::Focuser::ISNewSwitch(dev, name, states, names, n);
}

IPState IndiBuinskiiFocuser::MoveAbsFocuser(uint32_t targetPosition) {
    client->usePort(PortFD);
    client->moveTo(targetPosition);
    return IPS_BUSY;
}

bool IndiBuinskiiFocuser::Handshake() {
    client->usePort(PortFD);
    return client->handshake() > 0;
}

bool IndiBuinskiiFocuser::SetFocuserSpeed(int speed) {
    client->usePort(PortFD);
    return client->setSpeed(speed) > 0;
}

bool IndiBuinskiiFocuser::AbortFocuser() {
    client->usePort(PortFD);
    int currentPosition = 0;

    if (client->halt(&currentPosition) > 0) {
        FocusAbsPosN->value = currentPosition;
        return true;
    }

    return false;
}

void IndiBuinskiiFocuser::TimerHit()
{
    if (!isConnected())
    {
        SetTimer(POLLMS);
        return;
    }

    client->usePort(PortFD);

    int currentPosition = 0;
    if (client->getPosition(&currentPosition) > 0) {
        IDSetNumber(&FocusAbsPosNP, nullptr);
        FocusAbsPosN->value = currentPosition;
    }

    int currentSpeed = 0;
    if (client->getSpeed(&currentSpeed) > 0) {
        IDSetNumber(&FocusSpeedNP, nullptr);
        FocusSpeedN->value = currentSpeed;
    }

    if (FocusAbsPosNP.s == IPS_BUSY)
    {
        int isMoving = 0;
        client->isMoving(&isMoving);
        if (isMoving <= 0)
        {
            FocusAbsPosNP.s = IPS_OK;
            IDSetNumber(&FocusAbsPosNP, nullptr);
        }
    }

    SetTimer(POLLMS);
}
