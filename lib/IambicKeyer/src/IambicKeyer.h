#pragma once

#include "common.h"
#include "SymbolPlayer.h"

enum LeverState
{
  LEVER_UNSET = 0,
  LEVER_DOT,
  LEVER_DASH,
  LEVER_DOT_DASH,
  LEVER_DASH_DOT,
};

enum KeyerState
{
  KEYER_STATE_UNSET = 0,
  KEYER_STATE_STOPPED,
  KEYER_STATE_DOT,
  KEYER_STATE_DASH,
  KEYER_STATE_ALTERNATING_DOT,
  KEYER_STATE_ALTERNATING_DASH
};

const static char *lever_state_str[] = {
    "unset",
    "dot",
    "dash",
    "dot-dash",
    "dash-dot"};

const static char *keyer_state_str[] = {
    "unset",
    "stopped",
    "dot",
    "dash",
    "dash-alt",
    "dot-alt"};

class IambicKeyer
{
public:
  IambicKeyer(unsigned long duration_unit, play_state_changed_fun_ptr state_changed_cb, millis_fun_ptr millis_cb, bool mode_a);
  void tick();
  void playDash();
  void playDot();
  void play(LeverState lever_state);
  void stop();
  void setDurationUnit(unsigned long duration_unit);
  void setSpeedWPM(unsigned long speed_wpm);
  KeyerState nextKeyerState();
  void setLeverState(LeverState lever_state);
  bool isPlayStateExpired();
  void switchPlayStateIfReady();
  void setModeA(bool mode_a);
  bool getModeA();

private:
  SymbolPlayer symbol_player;
  KeyerState keyer_state;
  millis_fun_ptr millis_cb;
  LeverState lever_state = LEVER_UNSET;
  LeverState prev_lever_state = LEVER_UNSET;
  bool lever_upgrade = false;
  bool mode_a = false;
};
