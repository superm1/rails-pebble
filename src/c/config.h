/*
 * Rails for Pebble - Configuration header
 *
 * AppMessage key definitions for config exchange with companion app.
 */

#pragma once

#include <pebble.h>
#include "rails.h"

// Config keys for AppMessage
#define CONFIG_KEY_THEME 0x00
#define CONFIG_KEY_TIME_FORMAT 0x01
#define CONFIG_KEY_FIELD_1 0x10
#define CONFIG_KEY_FIELD_2 0x11
#define CONFIG_KEY_FIELD_3 0x12
#define CONFIG_KEY_FIELD_4 0x13
#define CONFIG_KEY_FIELD_5 0x14
#define CONFIG_KEY_FIELD_6 0x15
#define CONFIG_KEY_FIELD_7 0x16
#define CONFIG_KEY_FIELD_8 0x17
#define CONFIG_KEY_GAUGE_1 0x20
#define CONFIG_KEY_GAUGE_2 0x21
#define CONFIG_KEY_SHOW_PHONE 0x30

void config_load(RailsState *state);
void config_save(RailsState *state);
void config_cycle_theme(RailsState *state);
