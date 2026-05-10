/*
 * Rails for Pebble - Weather header
 */

#pragma once

#include <pebble.h>
#include "rails.h"

void weather_parse_from_dict(DictionaryIterator *iter, RailsState *state);
void weather_request_update(RailsState *state);
