/*
 * Rails for Pebble - Gauge module
 *
 * Renders horizontal rail-style progress bars with configurable maximums.
 * SDK 4 compatible.
 */

#include "rails.h"

static uint32_t calculate_auto_steps_goal(void) {
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);

    tick_time->tm_hour = 0;
    tick_time->tm_min = 0;
    tick_time->tm_sec = 0;
    time_t start_of_today = mktime(tick_time);
    time_t end_of_today = start_of_today + SECONDS_PER_DAY;

    HealthValue avg_steps = health_service_sum_averaged(
        HealthMetricStepCount,
        start_of_today,
        end_of_today,
        HealthServiceTimeScopeDaily
    );

    uint32_t goal = (uint32_t)(avg_steps * 1.1f);
    if (goal < 5000) goal = 5000;

    APP_LOG(APP_LOG_LEVEL_INFO, "Steps: 7-day avg=%d, goal=%lu", (int)avg_steps, (unsigned long)goal);

    return goal;
}

static uint32_t get_gauge_maximum(RailsState *state, GaugeTypeId gauge_type) {
    switch (gauge_type) {
        case GAUGE_STEPS:
            if (state->config.gauge_max.steps == 0) {
                return calculate_auto_steps_goal();
            }
            return state->config.gauge_max.steps;

        case GAUGE_CALORIES:
            return state->config.gauge_max.calories;

        case GAUGE_DISTANCE:
            return state->config.gauge_max.distance;

        case GAUGE_BATTERY:
            return 100;

        default:
            return 100;
    }
}

void gauges_draw(Layer *layer, GContext *ctx) {
    RailsState *state = g_rails_state;
    GRect bounds = layer_get_bounds(layer);

    int gauge_index = -1;
    for (int i = 0; i < 2; i++) {
        if (state->gauge_layers[i] == layer) {
            gauge_index = i;
            break;
        }
    }
    if (gauge_index < 0) return;

    GaugeTypeId gauge_type = (gauge_index == 0) ? state->config.gauge_1 : state->config.gauge_2;
    if (gauge_type == GAUGE_NONE) return;

    graphics_context_set_fill_color(ctx, state->current_theme_palette.gaugeBgColor);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);

    uint32_t maximum = get_gauge_maximum(state, gauge_type);
    float progress = 0.0f;
    switch (gauge_type) {
        case GAUGE_STEPS:
            progress = (float)state->health.steps / (float)maximum;
            break;
        case GAUGE_CALORIES:
            progress = (float)state->health.calories / (float)maximum;
            break;
        case GAUGE_DISTANCE:
            progress = state->health.distance_meters / (float)maximum;
            break;
        case GAUGE_BATTERY:
            progress = (float)state->battery.charge_percent / 100.0f;
            break;
        default:
            return;
    }

    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    int fill_w = (int)(bounds.size.w * progress);
    GRect fill_rect = GRect(bounds.origin.x, bounds.origin.y, fill_w, bounds.size.h);
    graphics_context_set_fill_color(ctx, state->current_theme_palette.gaugeColor);
    graphics_fill_rect(ctx, fill_rect, 0, GCornerNone);

    graphics_context_set_stroke_color(ctx, state->current_theme_palette.fgColor);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_rect(ctx, bounds);
}
