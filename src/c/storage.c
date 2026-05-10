/*
 * Rails for Pebble - Storage module
 *
 * Thin wrapper around Pebble persistent storage API.
 * SDK 4 compatible.
 */

#include "rails.h"

#define STORAGE_KEY_CONFIG 0x00

bool storage_save_config(RailsState *state) {
    return persist_write_data(STORAGE_KEY_CONFIG, &state->config, sizeof(RailsConfig));
}

bool storage_load_config(RailsState *state) {
    if (persist_exists(STORAGE_KEY_CONFIG)) {
        persist_read_data(STORAGE_KEY_CONFIG, &state->config, sizeof(RailsConfig));
        return true;
    }
    return false;
}

void storage_clear_all(void) {
    // SDK 4: no persist_clear(), delete keys individually
    for (uint32_t i = 0; i < 100; i++) {
        persist_delete(i);
    }
}
