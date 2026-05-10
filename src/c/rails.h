/*
 * Rails for Pebble - Main header
 *
 * Feature-rich watch face for Pebble Time 2 (Emery)
 * inspired by Garmin Rails.
 *
 * Target: Emery (Pebble Time 2) - 200x228 color display
 * SDK: Pebble SDK 4
 */

#pragma once

#include <pebble.h>
#include <math.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "message_keys.auto.h"

// ========================
// Display dimensions (Emery: 200x228)
// ========================
#define RAILS_DISPLAY_WIDTH 200
#define RAILS_DISPLAY_HEIGHT 228

// Layout zones (Garmin Rails-style layout)
#define ZONE_STATUS_ICONS_Y  16
#define ZONE_FIELD_ROW1_Y    40   // Top row of stats
#define ZONE_TIME_TOP        70   // Large time display
#define ZONE_TIME_HEIGHT     80
#define ZONE_GAUGE_Y         155  // Single rail gauge below time
#define ZONE_FIELD_ROW2_Y    170  // Bottom row of stats with icons
#define ZONE_DATE_Y          195  // Date at bottom
#define ZONE_ACTIVITY_ICONS_Y 215 // Small activity icons at very bottom

// Field slot dimensions (2 rows x 3 cols = 6 slots)
#define FIELD_SLOT_W 35
#define FIELD_SLOT_H 22
#define NUM_FIELDS 6

// Theme count

// ========================
// Theme colors
// ========================
typedef struct {
    GColor bgColor;
    GColor fgColor;
    GColor accentColor;
    GColor mutedColor;
    GColor gaugeColor;
    GColor gaugeBgColor;
} ThemePalette;

typedef enum {
    THEME_DARK = 0,
    THEME_LIGHT,
    THEME_LAST
} ThemeId;

typedef enum {
    ACCENT_BLUE = 0,
    ACCENT_GREEN,
    ACCENT_RED,
    ACCENT_AMBER,
    ACCENT_PURPLE,
    ACCENT_CYAN,
    ACCENT_LAST
} AccentColorId;

// ========================
// Configurable data field types
// ========================
typedef enum {
    FIELD_NONE = 0,
    FIELD_STEPS,
    FIELD_CALORIES,
    FIELD_DISTANCE,
    FIELD_HEART_RATE,
    FIELD_WEATHER,  // Shows icon + temperature together
    FIELD_BATTERY,
    FIELD_LAST
} FieldTypeId;

// ========================
// Gauge types
// ========================
typedef enum {
    GAUGE_NONE = 0,
    GAUGE_STEPS,
    GAUGE_CALORIES,
    GAUGE_DISTANCE,
    GAUGE_BATTERY,
    GAUGE_LAST
} GaugeTypeId;

// ========================
// Time format
// ========================
typedef enum {
    TIME_12H = 0,
    TIME_24H
} TimeFormat;

// ========================
// Temperature unit
// ========================
typedef enum {
    TEMP_CELSIUS = 0,
    TEMP_FAHRENHEIT
} TempUnit;

// ========================
// Distance unit
// ========================
typedef enum {
    DIST_KILOMETERS = 0,
    DIST_MILES
} DistUnit;


// ========================
// Gauge maximums
// ========================
typedef struct {
    uint32_t steps;          // Steps goal (0 = auto: 7-day avg + 10%)
    uint32_t calories;       // Calories goal
    uint32_t distance;       // Distance goal in meters
} GaugeMaximums;

// ========================
// Configuration
// ========================
typedef struct {
    ThemeId theme;
    AccentColorId accent_color;
    TimeFormat time_format;
    TempUnit temp_unit;
    DistUnit dist_unit;
    FieldTypeId fields[NUM_FIELDS];
    GaugeTypeId gauge_1;
    GaugeTypeId gauge_2;
    GaugeMaximums gauge_max;
    bool show_phone;
} RailsConfig;

// ========================
// Health data
// ========================
typedef struct {
    uint32_t steps;
    uint32_t calories;
    float distance_meters;
    uint16_t heart_rate;
    bool hr_available;
} HealthData;

// ========================
// Weather state
// ========================
typedef struct {
    int8_t temp_celsius;
} WeatherState;

// ========================
// Main state structure
// ========================
typedef struct {
    // Window
    Window *window;
    Layer *root_layer;
    StatusBarLayer *status_bar;

    // Layers
    Layer *time_layer;  // Custom layer for fctx rendering
    Layer *date_layer;  // Custom layer for date with icon background
    TextLayer *field_layers[NUM_FIELDS];

    // Gauge layers (custom draw layers)
    Layer *gauge_layers[2];

    // Icon bitmap layers - status icons at top
    BitmapLayer *phone_icon_layer;

    // Field icon layers - icons next to data fields
    Layer *field_icon_layers[NUM_FIELDS];

    // State
    ThemeId current_theme;
    ThemePalette current_theme_palette;
    RailsConfig config;
    HealthData health;
    BatteryChargeState battery;
    bool phone_connected;
    WeatherState weather;

    // Text buffers (must persist for text_layer_set_text)
    char time_buffer[16];
    char date_buffer[32];
    char field_buffers[NUM_FIELDS][16];

    // Icon resources (PDC vector and PNG bitmap)
    GDrawCommandImage *step_icon;
    GDrawCommandImage *heart_icon;
    GDrawCommandImage *battery_icon;
    GDrawCommandImage *battery_charging_icon;
    GDrawCommandImage *date_bg_icon;
    GDrawCommandImage *calorie_icon;
    GDrawCommandImage *distance_icon;
    GDrawCommandImage *bluetooth_icon;
    GBitmap *phone_icon_bitmap;

    // Weather icons (PDC vector)
    GDrawCommandImage *weather_clear_day_icon;
    GDrawCommandImage *weather_clear_night_icon;
    GDrawCommandImage *weather_cloudy_icon;
    GDrawCommandImage *weather_partly_cloudy_icon;
    GDrawCommandImage *weather_partly_cloudy_night_icon;
    GDrawCommandImage *weather_rain_light_icon;
    GDrawCommandImage *weather_rain_heavy_icon;
    GDrawCommandImage *weather_snow_light_icon;
    GDrawCommandImage *weather_snow_heavy_icon;
    GDrawCommandImage *weather_thunderstorm_icon;
    GDrawCommandImage *weather_rain_and_snow_icon;
    GDrawCommandImage *weather_generic_icon;
    GDrawCommandImage *current_weather_icon;  // Pointer to currently selected icon

    // Font resources (fctx vector fonts)
    FFont *leco_font;
    FFont *avenir_regular_font;
    FFont *avenir_bold_font;
} RailsState;

// Global state pointer for layer callbacks (SDK 4 pattern)
extern RailsState *g_rails_state;

// ========================
// Function prototypes
// ========================

// Main lifecycle
void rails_init(RailsState *state);
void rails_deinit(RailsState *state);

// Display
void display_set_theme(RailsState *state, ThemeId theme);
void display_update_time(RailsState *state, struct tm *tick_time, TimeUnits units_changed);
void display_update_date(RailsState *state, struct tm *tick_time);

// Fields
void fields_update_health(RailsState *state);
void fields_update_heart_rate(RailsState *state);

// Gauges
void gauges_draw(Layer *layer, GContext *ctx);

// Config
void config_load(RailsState *state);
void config_save(RailsState *state);
void config_cycle_theme(RailsState *state);

// Icons
void icons_update_phone(RailsState *state, bool connected);

// Weather
void weather_parse_from_dict(DictionaryIterator *iter, RailsState *state);

// Redraw helper
void rails_redraw(RailsState *state);
