#pragma once

#include "IambicKeyer.h"

enum PaddleState
{

};

enum LeverEvent
{
    LEVER_EVENT_DOT_OFF = 0,
    LEVER_EVENT_DOT_ON,
    LEVER_EVENT_DASH_OFF,
    LEVER_EVENT_DASH_ON
};

typedef unsigned long (*millis_fun_ptr)();
typedef void (*lever_state_changed_fun_ptr)(LeverState);

class PaddleCtl
{
public:
    PaddleCtl(unsigned long duration_unit, lever_state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb);
    void tick();
    void setPaddleState(PaddleState paddle_state);
    void setDotPushed(bool pushed);
    void setDashPushed(bool pushed);
    void onLeverEvent(LeverEvent lever_event);
    LeverState getNextLeverState(LeverEvent lever_event);
    bool nextStateReady();

private:
    millis_fun_ptr millis_cb;
    unsigned long state_change_threshold;
    lever_state_changed_fun_ptr state_changed_cb;
    LeverState current_lever_state;
    LeverState next_lever_state;
    unsigned long next_lever_state_changed = 0;
};
