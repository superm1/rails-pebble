/*
 * Rails for Pebble - Display module
 *
 * Handles time display, date display, and theme management.
 * SDK 4 compatible.
 */

#include <string.h>
#include "rails.h"

extern ThemePalette get_theme_palette(ThemeId theme, AccentColorId accent);

void display_set_theme(RailsState *state, ThemeId theme) {
    if (theme >= THEME_LAST) return;

    state->config.theme = theme;
    state->current_theme_palette = get_theme_palette(theme, state->config.accent_color);

    if (state->window) {
        window_set_background_color(state->window, state->current_theme_palette.bgColor);
    }

    if (!state->time_layer) return;

    for (int i = 0; i < NUM_FIELDS; i++) {
        text_layer_set_text_color(state->field_layers[i], state->current_theme_palette.fgColor);
        text_layer_set_background_color(state->field_layers[i], state->current_theme_palette.bgColor);
    }

    if (state->status_bar) {
        status_bar_layer_set_colors(state->status_bar,
                                    state->current_theme_palette.bgColor,
                                    state->current_theme_palette.fgColor);
    }

    rails_redraw(state);
}

void display_update_time(RailsState *state, struct tm *tick_time, TimeUnits units_changed) {
    if (!(units_changed & (HOUR_UNIT | MINUTE_UNIT))) return;

    char new_time_buffer[8];
    if (state->config.time_format == TIME_12H || !clock_is_24h_style()) {
        strftime(new_time_buffer, sizeof(new_time_buffer), "%I:%M", tick_time);
    } else {
        strftime(new_time_buffer, sizeof(new_time_buffer), "%H:%M", tick_time);
    }

    // Only repaint if the time string actually changed
    if (strcmp(state->time_buffer, new_time_buffer) != 0) {
        strncpy(state->time_buffer, new_time_buffer, sizeof(state->time_buffer));
        layer_mark_dirty(state->time_layer);
    }
}

void display_update_date(RailsState *state, struct tm *tick_time) {
    // Only update date when it actually changes (day changes)
    static int last_day = -1;
    if (tick_time->tm_mday != last_day) {
        last_day = tick_time->tm_mday;
        layer_mark_dirty(state->date_layer);
    }
}
