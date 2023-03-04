#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "IambicKeyer.h"
#include "MorseDecoder.hpp"
#include "MultirowScroller.hpp"
#include "PaddleCtl.h"

#include "soundsynth.h"
#include "button.h"

#include "config.h"

unsigned long unit_duration = 100;
unsigned long decode_threshold = unit_duration * 3;

const uint8_t sound_timer_num = 1;

uint8_t dot_in_pin = DOT_IN_PIN;
uint8_t dash_in_pin = DASH_IN_PIN;

uint8_t speed_wpm = 10;
uint8_t speed_wpm_pin = 32;

bool mode_a = false;
uint8_t mode_a_pin = 33;

const int rows = 2;
const int cols = 16;

const size_t bufSz = rows * cols;
char buf[bufSz];

char topRow[cols + 1];
char bottomRow[cols + 1];

bool dot_btn_pushed = false;
bool dash_btn_pushed = false;

bool ready_to_switch = false;

unsigned long last_settings_read = 0;
unsigned long settings_read_interval = 1000;

PlayState previous_play_state = PLAY_STATE_UNSET;

LiquidCrystal_I2C lcd(0x27, 16, 2);
MultirowScroller lcd_scroller(buf, bufSz, rows, cols);

void letter_decoded_cb(char letter)
{
  lcd_scroller.push(letter);
  lcd_scroller.getRow(0, topRow);
  lcd_scroller.getRow(1, bottomRow);

  // lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(topRow);

  lcd.setCursor(0, 1);
  lcd.print(bottomRow);
}

MorseDecoder morse_decoder(decode_threshold, &letter_decoded_cb, &millis);

void play_state_changed_cb(PlayState play_state)
{
  if (!(previous_play_state == play_state && play_state == 1))
  {
    // Serial.printf("previous state %d, %d current state\r\n", previous_play_state, play_state);
  }
  switch (play_state)
  {
  case PLAY_STATE_DOT_ON:
  case PLAY_STATE_DASH_ON:
    morse_decoder.setTransmitting(true);
    soundsynth_note_on();

    if (play_state != previous_play_state)
    {
      if (play_state == PLAY_STATE_DOT_ON)
      {
        Serial.printf(".");
        morse_decoder.appendDot();
      }
      if (play_state == PLAY_STATE_DASH_ON)
      {
        Serial.printf("-");
        morse_decoder.appendDash();
      }
    }
    break;
  case PLAY_STATE_DOT_OFF:
  case PLAY_STATE_DASH_OFF:
    soundsynth_note_off();
    break;
  case PLAY_STATE_STOPPED:
  case PLAY_STATE_UNSET:
    morse_decoder.setTransmitting(false);
    soundsynth_note_off();
    break;
  }
  previous_play_state = play_state;
}

IambicKeyer keyer(unit_duration, &play_state_changed_cb, &millis, false);

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();

  pinMode(dot_in_pin, INPUT_PULLUP);
  pinMode(dash_in_pin, INPUT_PULLUP);

  soundsynth_config_t conf = {
      .frequency = SOUND_FREQUENCY,
      .timer_num = sound_timer_num,
      .pin = DAC_OUT_PIN};

  soundsynth_init(&conf);
}

int incomingByte = 0;

void lever_state_changed_cb(LeverState lever_state)
{
  keyer.setLeverState(lever_state);
}

PaddleCtl paddle_ctl(10, lever_state_changed_cb, &millis);

void loop()
{
  unsigned long curr_time = millis();

  if (last_settings_read == 0 || (curr_time - last_settings_read) > settings_read_interval)
  {
    speed_wpm = map(analogRead(speed_wpm_pin), 0, 1023, 10, 60);
    keyer.setSpeedWPM(speed_wpm);
    mode_a = digitalRead(mode_a_pin) == LOW;
    keyer.setModeA(mode_a);
    last_settings_read = curr_time;
    printf("speed wpm is: %d\n", speed_wpm);
    printf("mode a is: %d\n", mode_a);
  }

  dot_btn_pushed = digitalRead(dot_in_pin) == LOW;
  dash_btn_pushed = digitalRead(dash_in_pin) == LOW;

  paddle_ctl.setDotPushed(dot_btn_pushed);
  paddle_ctl.setDashPushed(dash_btn_pushed);

  paddle_ctl.tick();
  keyer.tick();
  morse_decoder.tick();
}
