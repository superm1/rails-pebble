/*
 * Rails for Pebble - Icons header
 */

#pragma once

#include <pebble.h>
#include "rails.h"

void icons_update_phone(RailsState *state, bool connected);
void draw_phone_icon(Layer *layer, GContext *ctx);
