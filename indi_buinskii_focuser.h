
#ifndef IndiBuinskiiFocuser_H
#define IndiBuinskiiFocuser_H

#include <indidevapi.h>
#include <indicom.h>
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

        const char *getDefaultName() override;
        virtual bool Handshake() override;
        virtual bool initProperties() override;
        virtual bool updateProperties() override;
        virtual IPState MoveAbsFocuser(uint32_t targetPosition) override;
        virtual bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n) override;
        virtual bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n) override;

        virtual bool SetFocuserSpeed(int speed) override;
        virtual bool SetFocuserAcceleration(int accel);
        virtual bool SyncFocuser(uint32_t ticks) override;
        virtual bool AbortFocuser() override;
        virtual void TimerHit() override;
};

#endif
