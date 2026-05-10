/*
 * Rails for Pebble - Configuration module
 *
 * Handles app configuration loading, saving, and theme cycling.
 * SDK 4 compatible.
 */

#include "rails.h"

void config_load(RailsState *state) {
    if (persist_exists(0x00)) {
        persist_read_data(0x00, &state->config, sizeof(RailsConfig));
    } else {
        state->config.theme = THEME_LIGHT;
        state->config.accent_color = ACCENT_BLUE;
        state->config.time_format = TIME_12H;
        state->config.temp_unit = TEMP_FAHRENHEIT;
        state->config.dist_unit = DIST_MILES;
        state->config.fields[0] = FIELD_STEPS;
        state->config.fields[1] = FIELD_CALORIES;
        state->config.fields[2] = FIELD_WEATHER;
        state->config.fields[3] = FIELD_HEART_RATE;
        state->config.fields[4] = FIELD_DISTANCE;
        state->config.fields[5] = FIELD_BATTERY;
        state->config.gauge_1 = GAUGE_STEPS;
        state->config.gauge_2 = GAUGE_CALORIES;
        state->config.gauge_max.steps = 0;
        state->config.gauge_max.calories = 2000;
        state->config.gauge_max.distance = 8000;
        state->config.show_phone = true;
    }

    if (state->phone_icon_layer) {
        layer_set_hidden(bitmap_layer_get_layer(state->phone_icon_layer), !state->config.show_phone);
    }
}

void config_save(RailsState *state) {
    persist_write_data(0x00, &state->config, sizeof(RailsConfig));
}

void config_cycle_theme(RailsState *state) {
    state->config.theme = (ThemeId)((state->config.theme + 1) % THEME_LAST);
    config_save(state);
    display_set_theme(state, state->config.theme);
}
