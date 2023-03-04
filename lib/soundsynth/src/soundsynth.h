#pragma once

typedef struct
{
    uint16_t frequency;
    uint8_t timer_num;
    uint8_t pin;
} soundsynth_config_t;

esp_err_t soundsynth_init(const soundsynth_config_t *soundsynth_config);
void soundsynth_note_on(void);
void soundsynth_note_off(void);
bool soundsynth_is_note_on(void);
