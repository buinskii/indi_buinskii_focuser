
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
    FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_ABORT | FOCUSER_HAS_VARIABLE_SPEED | FOCUSER_CAN_SYNC);
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

    setDefaultPollingPeriod(1000);

    IUFillNumber(&FocusAccelerationN[0], "FOCUS_ACCELERATION_VALUE", "Focus Acceleration", "%3.0f", 0.0, 100.0, 1.0, 100.0);
    IUFillNumberVector(&FocusAccelerationNP, FocusAccelerationN, 1, m_defaultDevice->getDeviceName(), "FOCUS_ACCELERATION", "Acceleration", MAIN_CONTROL_TAB, IP_RW, 60, IPS_OK);

    return true;
}

bool IndiBuinskiiFocuser::updateProperties() {
    INDI::Focuser::updateProperties();

    if (isConnected()) {
        defineNumber(&FocusAccelerationNP);
    }
    else {
        deleteProperty(FocusAccelerationNP.name);
    }

    return true;
}

bool IndiBuinskiiFocuser::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) {
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (!strcmp(name, FocusAccelerationNP.name))
        {
            FocusAccelerationNP.s    = IPS_OK;
            int current_acceleration = FocusAccelerationN[0].value;
            IUUpdateNumber(&FocusAccelerationNP, values, names, n);

            if (SetFocuserAcceleration(FocusAccelerationN[0].value) == false)
            {
                FocusAccelerationN[0].value = current_acceleration;
                FocusAccelerationNP.s       = IPS_ALERT;
            }

            //  Update client display
            IDSetNumber(&FocusAccelerationNP, "Focuser acceleration updated to %d", (int)FocusAccelerationN[0].value);
            return true;
        }
    }

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

bool IndiBuinskiiFocuser::SetFocuserAcceleration(int accel) {
    client->usePort(PortFD);
    return client->setAcceleration(accel) > 0;
}

bool IndiBuinskiiFocuser::SyncFocuser(uint32_t ticks) {
    client->usePort(PortFD);
    return client->setPosition(ticks) > 0;
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
        if (FocusAbsPosN->value != currentPosition) {
            FocusAbsPosN->value = currentPosition;
            IDSetNumber(&FocusAbsPosNP, nullptr);
        }
    }

    int currentSpeed = 0;
    if (client->getSpeed(&currentSpeed) > 0) {
        if (FocusSpeedN->value != currentSpeed) {
            FocusSpeedN->value = currentSpeed;
            IDSetNumber(&FocusSpeedNP, nullptr);
        }
    }

    int currentAcceleration = 0;
    if (client->getAcceleration(&currentAcceleration) > 0) {
        if (FocusAccelerationN->value != currentAcceleration) {
            FocusAccelerationN->value = currentAcceleration;
            IDSetNumber(&FocusAccelerationNP, nullptr);
        }
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
