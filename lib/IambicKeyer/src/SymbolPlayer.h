#include "common.h"

enum PlayState
{
    PLAY_STATE_UNSET = 0,
    PLAY_STATE_STOPPED,
    PLAY_STATE_DOT_ON,
    PLAY_STATE_DOT_OFF,
    PLAY_STATE_DASH_ON,
    PLAY_STATE_DASH_OFF
};

const static char *symbol_player_state_str[] = {
    "unset",
    "stopped",
    "dot_on",
    "dot_off",
    "dash_on",
    "dash_off"};

typedef void (*play_state_changed_fun_ptr)(PlayState);

class SymbolPlayer
{
public:
    SymbolPlayer(unsigned long duration_unit, play_state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb);
    bool ready();
    void playDot();
    void playDash();
    void tick();
    void setDurationUnit(unsigned long duration_unit);

protected:
    PlayState getPlayState();
    unsigned long getPlayStateAge();
    PlayState nextPlayState();
    bool playStateFinished();
    unsigned long getPlayStateDuration();
    void setPlayState(PlayState state);

private:
    millis_fun_ptr millis_cb;
    play_state_changed_fun_ptr state_changed_cb;
    PlayState play_state = PLAY_STATE_UNSET;
    unsigned long last_state_change = 0;
    unsigned long int duration_unit;
};
