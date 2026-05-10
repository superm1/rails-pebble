/*
 * Rails for Pebble - Data fields module
 *
 * SDK 4 compatible - uses HealthMetric enum and HealthValue (int32_t)
 */

#include "rails.h"

void fields_update_health(RailsState *state) {
    HealthValue steps = health_service_sum_today(HealthMetricStepCount);
    HealthValue calories = health_service_sum_today(HealthMetricActiveKCalories);
    HealthValue distance = health_service_sum_today(HealthMetricWalkedDistanceMeters);

    state->health.steps = (uint32_t)steps;
    state->health.calories = (uint32_t)calories;
    state->health.distance_meters = (float)distance;
    // Note: SDK 4 has no floors metric available

    for (int i = 0; i < NUM_FIELDS; i++) {
        switch (state->config.fields[i]) {
            case FIELD_STEPS: {
                // Format like TimeStyle: <1000 shows full, 1000-9999 shows "5.4k", >=10000 shows "12k"
                uint32_t steps = state->health.steps;
                if (steps < 1000) {
                    snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                             "%lu", (unsigned long)steps);
                } else if (steps < 10000) {
                    uint32_t thousands = steps / 1000;
                    uint32_t hundreds = (steps / 100) % 10;
                    snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                             "%lu.%luk", (unsigned long)thousands, (unsigned long)hundreds);
                } else {
                    uint32_t thousands = steps / 1000;
                    snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                             "%luk", (unsigned long)thousands);
                }
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
                break;
            }
            case FIELD_CALORIES: {
                // Format like steps: <1000 shows full, 1000-9999 shows "2.5k", >=10000 shows "12k"
                uint32_t calories = state->health.calories;
                if (calories < 1000) {
                    snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                             "%lu", (unsigned long)calories);
                } else if (calories < 10000) {
                    uint32_t thousands = calories / 1000;
                    uint32_t hundreds = (calories / 100) % 10;
                    snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                             "%lu.%luk", (unsigned long)thousands, (unsigned long)hundreds);
                } else {
                    uint32_t thousands = calories / 1000;
                    snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                             "%luk", (unsigned long)thousands);
                }
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
                break;
            }
            case FIELD_DISTANCE: {
                // Pebble SDK doesn't support floating point in snprintf
                float distance;
                if (state->config.dist_unit == DIST_MILES) {
                    // Convert meters to miles (1 mile = 1609.34 meters)
                    distance = state->health.distance_meters / 1609.34;
                } else {
                    // Kilometers
                    distance = state->health.distance_meters / 1000.0;
                }
                uint32_t whole = (uint32_t)distance;
                uint32_t frac = (uint32_t)((distance - whole) * 10);
                snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                         "%lu.%lu", (unsigned long)whole, (unsigned long)frac);
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
                break;
            }
            case FIELD_BATTERY:
                snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                         "%d%%", state->battery.charge_percent);
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
                break;
            case FIELD_WEATHER: {
                // Show temperature text (icon is shown separately via field icon layer)
                int temp = state->weather.temp_celsius;
                if (state->config.temp_unit == TEMP_FAHRENHEIT) {
                    temp = (temp * 9 / 5) + 32;
                }
                snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                         "%d\u00B0", temp);
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
                break;
            }
            default:
                break;
        }
    }
}

void fields_update_heart_rate(RailsState *state) {
    HealthValue hr = health_service_peek_current_value(HealthMetricHeartRateBPM);
    if (hr != 0) {
        state->health.heart_rate = (uint16_t)hr;
        state->health.hr_available = true;
        for (int i = 0; i < NUM_FIELDS; i++) {
            if (state->config.fields[i] == FIELD_HEART_RATE) {
                snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                         "%hu", (unsigned short)hr);
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
            }
        }
    }
}
