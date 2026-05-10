/*
 * Rails for Pebble - Icons module (SDK 4)
 *
 * Handles status icon rendering using canvas-drawn icons.
 * SDK 4 compatible - uses GColor8 directly, compound literals for GRect.
 */

#include "rails.h"
#include "util.h"

void draw_phone_icon(Layer *layer, GContext *ctx) {
    RailsState *state = g_rails_state;
    if (!state || !state->bluetooth_icon) return;

    gdraw_command_image_recolor(state->bluetooth_icon,
                                state->current_theme_palette.fgColor,
                                state->current_theme_palette.fgColor);
    gdraw_command_image_draw(ctx, state->bluetooth_icon, GPoint(0, 0));
}

void icons_update_phone(RailsState *state, bool connected) {
    if (state->phone_icon_layer) {
        bool should_show = state->config.show_phone && connected;
        layer_set_hidden((Layer *)state->phone_icon_layer, !should_show);
    }
}
