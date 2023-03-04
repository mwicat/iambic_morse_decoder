#include "PaddleCtl.h"

PaddleCtl::PaddleCtl(unsigned long state_change_threshold, lever_state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb)
{
    this->state_change_threshold = state_change_threshold;
    this->state_changed_cb = state_changed_cb;
    this->millis_cb = millis_cb;
}

void PaddleCtl::setDotPushed(bool pushed)
{
    onLeverEvent(pushed ? LEVER_EVENT_DOT_ON : LEVER_EVENT_DOT_OFF);
}

void PaddleCtl::setDashPushed(bool pushed)
{
    onLeverEvent(pushed ? LEVER_EVENT_DASH_ON : LEVER_EVENT_DASH_OFF);
}

void PaddleCtl::onLeverEvent(LeverEvent lever_event)
{
    LeverState new_next_lever_state = getNextLeverState(lever_event);
    if (next_lever_state == new_next_lever_state)
    {
        return;
    }
    next_lever_state = new_next_lever_state;
    next_lever_state_changed = millis_cb();
}

LeverState PaddleCtl::getNextLeverState(LeverEvent lever_event)
{
    switch (next_lever_state)
    {
    case LEVER_UNSET:
        switch (lever_event)
        {
        case LEVER_EVENT_DOT_ON:
            return LEVER_DOT;
        case LEVER_EVENT_DASH_ON:
            return LEVER_DASH;
        default:
            return next_lever_state;
        }
    case LEVER_DOT:
        switch (lever_event)
        {
        case LEVER_EVENT_DASH_ON:
            return LEVER_DOT_DASH;
        case LEVER_EVENT_DOT_OFF:
            return LEVER_UNSET;
        default:
            return next_lever_state;
        }
    case LEVER_DASH:
        switch (lever_event)
        {
        case LEVER_EVENT_DOT_ON:
            return LEVER_DASH_DOT;
        case LEVER_EVENT_DASH_OFF:
            return LEVER_UNSET;
        default:
            return next_lever_state;
        }
    case LEVER_DASH_DOT:
    case LEVER_DOT_DASH:
        switch (lever_event)
        {
        case LEVER_EVENT_DOT_OFF:
            return LEVER_DASH;
        case LEVER_EVENT_DASH_OFF:
            return LEVER_DOT;
        default:
            return next_lever_state;
        }
    default:
        return next_lever_state;
    }
}

bool PaddleCtl::nextStateReady()
{
    if (next_lever_state_changed == 0)
    {
        return false;
    }
    return (millis_cb() - next_lever_state_changed) > state_change_threshold;
}

void PaddleCtl::tick()
{
    if (nextStateReady())
    {
        next_lever_state_changed = 0;
        current_lever_state = next_lever_state;
        state_changed_cb(current_lever_state);
    }
}
