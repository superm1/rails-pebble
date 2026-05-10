/*
 * Rails for Pebble - Display module
 *
 * Handles time display, date display, and theme management.
 * SDK 4 compatible.
 */

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
    if (!(units_changed & (HOUR_UNIT | MINUTE_UNIT | SECOND_UNIT))) return;

    if (state->config.time_format == TIME_12H || !clock_is_24h_style()) {
        strftime(state->time_buffer, sizeof(state->time_buffer), "%I:%M", tick_time);
    } else {
        strftime(state->time_buffer, sizeof(state->time_buffer), "%H:%M", tick_time);
    }
    layer_mark_dirty(state->time_layer);
}

void display_update_date(RailsState *state, struct tm *tick_time) {
    layer_mark_dirty(state->date_layer);
}
