
#ifndef IndiBuinskiiFocuser_H
#define IndiBuinskiiFocuser_H

#include <indifocuser.h>
#include "buinskii_focuser_serial_client.h"

class IndiBuinskiiFocuser : public INDI::Focuser
{
    protected:
    private:
        BuinskiiFocuserSerialClient *client = nullptr;
        INumberVectorProperty FocusAccelerationNP;
        INumber FocusAccelerationN[1];

    public:
        IndiBuinskiiFocuser();
        virtual ~IndiBuinskiiFocuser();

        const char *getDefaultName();
        virtual bool Handshake();
        virtual bool initProperties();
        virtual bool updateProperties();
        virtual IPState MoveAbsFocuser(uint32_t targetPosition);
        virtual bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
        virtual bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);

        virtual bool SetFocuserSpeed(int speed);
        virtual bool SetFocuserAcceleration(int accel);
        virtual bool SyncFocuser(uint32_t ticks);
        virtual bool AbortFocuser();
        virtual void TimerHit();
};

#endif
