#include "SymbolPlayer.h"

SymbolPlayer::SymbolPlayer(unsigned long duration_unit, play_state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb)
{
    this->duration_unit = duration_unit;
    this->state_changed_cb = state_changed_cb;
    this->millis_cb = millis_cb;
}

bool SymbolPlayer::playStateFinished()
{
    switch (play_state)
    {
    case PLAY_STATE_UNSET:
        return true;
    default:
        return getPlayStateAge() > getPlayStateDuration();
    }
}

unsigned long SymbolPlayer::getPlayStateAge()
{
    return millis_cb() - last_state_change;
}

unsigned long SymbolPlayer::getPlayStateDuration()
{
    switch (play_state)
    {
    case PLAY_STATE_DASH_ON:
        return duration_unit * 3;
    case PLAY_STATE_DOT_ON:
    case PLAY_STATE_DOT_OFF:
    case PLAY_STATE_DASH_OFF:
        return duration_unit;
    case PLAY_STATE_STOPPED:
    case PLAY_STATE_UNSET:
        return 0;
    }
}

PlayState SymbolPlayer::nextPlayState()
{
    switch (play_state)
    {
    case PLAY_STATE_UNSET:
    case PLAY_STATE_STOPPED:
        return PLAY_STATE_STOPPED;
    case PLAY_STATE_DOT_ON:
        return playStateFinished() ? PLAY_STATE_DOT_OFF : PLAY_STATE_DOT_ON;
    case PLAY_STATE_DASH_ON:
        return playStateFinished() ? PLAY_STATE_DASH_OFF : PLAY_STATE_DASH_ON;
    case PLAY_STATE_DOT_OFF:
        return playStateFinished() ? PLAY_STATE_STOPPED : PLAY_STATE_DOT_OFF;
    case PLAY_STATE_DASH_OFF:
        return playStateFinished() ? PLAY_STATE_STOPPED : PLAY_STATE_DASH_OFF;
    }
}

void SymbolPlayer::tick()
{
    PlayState next_play_state = nextPlayState();
    setPlayState(next_play_state);
}

bool SymbolPlayer::ready()
{
    return (play_state == PLAY_STATE_UNSET || play_state == PLAY_STATE_STOPPED);
}

void SymbolPlayer::playDot()
{
    setPlayState(PLAY_STATE_DOT_ON);
}

void SymbolPlayer::playDash()
{
    setPlayState(PLAY_STATE_DASH_ON);
}

void SymbolPlayer::setPlayState(PlayState play_state)
{
    if (this->play_state == play_state)
    {
        return;
    }
    printf("   $$$ Play state: %s -> %s\n", symbol_player_state_str[this->play_state], symbol_player_state_str[play_state]);
    this->play_state = play_state;
    last_state_change = millis_cb();
    state_changed_cb(play_state);
}

void SymbolPlayer::setDurationUnit(unsigned long duration_unit)
{
    this->duration_unit = duration_unit;
}
