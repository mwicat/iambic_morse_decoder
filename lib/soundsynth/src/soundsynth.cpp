#include <math.h>
#include <Arduino.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

#include "freertos/event_groups.h"

#include "driver/dac_common.h"
#include "soc/dac_channel.h"

#include "esp_err.h"
#include "esp_check.h"

#include "soundsynth.h"
#include "wavetable.h"

static __attribute__((unused)) const char *TAG = "soundsynth";

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

uint32_t apb_frquency;

const uint16_t sample_rate = 44100;
const uint16_t divider = 8;

uint64_t alarm_value;

uint8_t dac_out_pin_num;

volatile size_t lut_jump;
volatile size_t lut_index = 0;

uint16_t gain_acc = 0;
uint16_t gain_acc_limit = sample_rate / 20;

volatile int note_on = false;
volatile boolean mute = false;

dac_channel_t dac_channel;

void IRAM_ATTR soundsynth_on_timer()
{
    portENTER_CRITICAL_ISR(&timerMux);
    lut_index = (lut_index + lut_jump) % wavetable_len;
    portEXIT_CRITICAL_ISR(&timerMux);

    if (note_on && gain_acc < gain_acc_limit)
    {
        mute = false;
        gain_acc = gain_acc + 1;
    }
    else if (!note_on && gain_acc > 0)
    {
        gain_acc = gain_acc - 1;
    }

    if (gain_acc == 0 && !mute)
    {
        mute = true;
        dac_output_voltage(dac_channel, 127);
    }

    if (gain_acc != 0)
    {
        dac_output_voltage(dac_channel, ((wavetable[lut_index] - 127) * gain_acc / gain_acc_limit * 0.7) + 127);
    }
}

esp_err_t soundsynth_init(const soundsynth_config_t *soundsynth_config)
{
    ESP_RETURN_ON_FALSE(
        soundsynth_config->pin >= DAC_CHANNEL_1_GPIO_NUM && soundsynth_config->pin <= DAC_CHANNEL_2_GPIO_NUM,
        ESP_ERR_INVALID_ARG, TAG, "DAC channel error");

    dac_channel = (dac_channel_t)(soundsynth_config->pin - DAC_CHANNEL_1_GPIO_NUM);

    apb_frquency = getApbFrequency();
    alarm_value = round((float)apb_frquency / divider / sample_rate - 1);
    lut_jump = round(soundsynth_config->frequency / ((float)sample_rate / wavetable_len));

    dac_output_enable(dac_channel);

    timer = timerBegin(soundsynth_config->timer_num, divider, true);

    timerAttachInterrupt(timer, &soundsynth_on_timer, true);
    timerAlarmWrite(timer, alarm_value, true);
    timerAlarmEnable(timer);

    return ESP_OK;
}

void soundsynth_note_on(void)
{
    note_on = true;
}

void soundsynth_note_off(void)
{
    note_on = false;
}
