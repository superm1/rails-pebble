/*
 * Rails for Pebble - Main header
 * A feature-rich watch face inspired by Garmin Rails
 * Target: Pebble Time 2 (Emery) - 144x168 color display
 */

#pragma once

#include <pebble.h>

// Display dimensions for Pebble Time 2
#define DISPLAY_WIDTH 144
#define DISPLAY_HEIGHT 168

// Theme definitions - 6 color palettes
typedef enum {
    THEME_DARK = 0,
    THEME_LIGHT,
    THEME_BLUE,
    THEME_GREEN,
    THEME_RED,
    THEME_AMBER,
    NUM_THEMES
} ThemeId;

typedef struct {
    GColor bgColor;
    GColor fgColor;
    GColor accentColor;
    GColor mutedColor;
    GColor gaugeColor;
    GColor gaugeBgColor;
} ThemePalette;

// Data field types - what each slot can display
typedef enum {
    FIELD_NONE = 0,
    FIELD_STEPS,
    FIELD_CALORIES,
    FIELD_DISTANCE,
    FIELD_FLOORS,
    FIELD_HEART_RATE,
    FIELD_WEATHER_TEMP,
    FIELD_WEATHER_ICON,
    FIELD_BATTERY,
    FIELD_PHONE_STATUS,
    NUM_FIELD_TYPES
} FieldTypeId;

// Gauge types
typedef enum {
    GAUGE_NONE = 0,
    GAUGE_STEPS,
    GAUGE_CALORIES,
    GAUGE_DISTANCE,
    GAUGE_FLOORS,
    GAUGE_BATTERY,
    NUM_GAUGE_TYPES
} GaugeTypeId;

// Time format options
typedef enum {
    TIME_12H = 0,
    TIME_24H
} TimeFormat;

// Configuration structure - all user settings
typedef struct {
    ThemeId theme;
    FieldTypeId fields[8];
    TimeFormat time_format;
    GaugeTypeId gauge_1;
    GaugeTypeId gauge_2;
    bool show_phone;
} AppConfig;

// Health data structure
typedef struct {
    int steps;
    int calories;
    float distance_meters;
    int floors;
    int heart_rate;
} HealthData;

// Connection state
typedef struct {
    bool phone_connected;
    bool pebblekit_connected;
} ConnectionData;

// Main state structure
typedef struct {
    Window *window;
    StatusBarLayer *status_bar;
    Layer *root_layer;

    // Time display
    TextLayer *time_layer;
    TextLayer *date_layer;

    // Data fields (8 slots)
    TextLayer *field_layers[8];
    GPoint field_positions[8];

    // Rail gauges (2)
    Layer *gauge_layers[2];
    GRect gauge_rects[2];

    // Icons
    BitmapLayer *phone_icon_layer;

    // State
    AppConfig config;
    HealthData health;
    ConnectionData connection;

    // Time
    struct tm *last_time;
    TimeUnits last_ticked_units;

    // Theme
    ThemePalette current_theme;
} RailsState;

// ========================
// Function declarations
// ========================

// Main entry
void rails_init(RailsState *state);
void rails_deinit(RailsState *state);

// Display
void display_update_time(RailsState *state, struct tm *tick_time, TimeUnits units_changed);
void display_update_date(RailsState *state, struct tm *tick_time);
void display_set_theme(RailsState *state, ThemeId theme);
void display_draw_time(RailsState *state, GContext *ctx, struct tm *tick_time);
void display_draw_date(RailsState *state, GContext *ctx, struct tm *tick_time);

// Fields
void fields_update_all(RailsState *state);
void fields_draw_slot(RailsState *state, GContext *ctx, int index);
void fields_update_health(RailsState *state);
void fields_update_heart_rate(RailsState *state);

// Gauges
void gauges_init(RailsState *state);
void gauges_deinit(RailsState *state);
void gauges_draw(RailsState *state, GContext *ctx, int gauge_index, GaugeTypeId type, int value, int max_value);
void gauges_update_all(RailsState *state);

// Config
void config_load(RailsState *state);
void config_save(RailsState *state);
void config_handle_message(RailsState *state, uint32_t key, const Tuple *tuple);
void config_cycle_theme(RailsState *state);

// Storage
bool storage_save_config(RailsState *state, const AppConfig *config);
bool storage_load_config(AppConfig *config);

// Weather
void weather_update(RailsState *state);
void weather_draw(RailsState *state, GContext *ctx);
void weather_parse_message(RailsState *state, const Tuple *tuple);

// Icons
void icons_update_phone(RailsState *state, const ConnectionData *conn);

// Status bar
void status_bar_update(RailsState *state);
