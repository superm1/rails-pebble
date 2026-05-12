/*
 * Rails for Pebble - Main watchface entry point
 * Target: Pebble Time 2 (Emery) - 200x228 color display
 * SDK: Pebble SDK 4
 */

#include "rails.h"
#include "config.h"
#include "weather.h"
#include "util.h"
#include "icons.h"

RailsState *g_rails_state;
const GColor accent_colors[] = {
    GColorFromRGB(0, 162, 232),
    GColorFromRGB(76, 175, 80),
    GColorFromRGB(220, 20, 60),
    GColorFromRGB(255, 183, 77),
    GColorFromRGB(156, 39, 176),
    GColorFromRGB(0, 188, 212),
};

ThemePalette get_theme_palette(ThemeId theme, AccentColorId accent) {
    ThemePalette palette;
    GColor accent_color = accent_colors[accent];

    if (theme == THEME_DARK) {
        palette.bgColor = GColorBlack;
        palette.fgColor = GColorWhite;
        palette.accentColor = accent_color;
        palette.mutedColor = GColorLightGray;
        palette.gaugeColor = accent_color;
        palette.gaugeBgColor = GColorDarkGray;
    } else {
        palette.bgColor = GColorWhite;
        palette.fgColor = GColorBlack;
        palette.accentColor = accent_color;
        palette.mutedColor = GColorDarkGray;
        palette.gaugeColor = accent_color;
        palette.gaugeBgColor = GColorLightGray;
    }

    return palette;
}

static const GPoint field_positions[NUM_FIELDS] = {
    {30, 32},   {96, 32},   {162, 32},
    {30, 164},  {96, 164},  {162, 164},
};

static const GRect gauge_rects[2] = {
    GRect(10, 62, 180, 6),
    GRect(10, 148, 180, 6),
};

static void time_layer_draw(Layer *layer, GContext *ctx) {
    RailsState *state = g_rails_state;
    if (!state || !state->leco_font) return;

    GRect bounds = layer_get_bounds(layer);

    FContext fctx;
    fctx_init_context(&fctx, ctx);

    FPoint time_pos;
    time_pos.x = INT_TO_FIXED(bounds.size.w / 2);
    time_pos.y = INT_TO_FIXED(110);

    fctx_begin_fill(&fctx);
    fctx_set_fill_color(&fctx, state->current_theme_palette.accentColor);
    fctx_set_text_em_height(&fctx, state->leco_font, 70);  // Increased from 54 to better fill space
    fctx_set_offset(&fctx, time_pos);
    fctx_draw_string(&fctx, state->time_buffer, state->leco_font,
                     GTextAlignmentCenter, FTextAnchorMiddle);
    fctx_end_fill(&fctx);

    fctx_deinit_context(&fctx);
}

static void date_layer_draw(Layer *layer, GContext *ctx) {
    RailsState *state = g_rails_state;
    if (!state) return;

    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    char day_name[4];
    char day_num[3];
    char month_name[10];

    strftime(day_name, sizeof(day_name), "%a", tick_time);
    strftime(day_num, sizeof(day_num), "%d", tick_time);
    strftime(month_name, sizeof(month_name), "%b", tick_time);

    graphics_context_set_text_color(ctx, state->current_theme_palette.fgColor);
    graphics_draw_text(ctx, day_name, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                      GRect(0, 2, 35, 20),
                      GTextOverflowModeFill, GTextAlignmentLeft, NULL);

    if (state->date_bg_icon) {
        gdraw_command_image_recolor(state->date_bg_icon,
                                    state->current_theme_palette.accentColor,
                                    state->current_theme_palette.fgColor);
        gdraw_command_image_draw(ctx, state->date_bg_icon,
                                GPoint(38, 0));
    }

    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, day_num, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                      GRect(38, 2, 26, 20),
                      GTextOverflowModeFill, GTextAlignmentCenter, NULL);

    graphics_context_set_text_color(ctx, state->current_theme_palette.fgColor);
    graphics_draw_text(ctx, month_name, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                      GRect(65, 2, 35, 20),
                      GTextOverflowModeFill, GTextAlignmentRight, NULL);
}

static void field_icon_draw(Layer *layer, GContext *ctx) {
    RailsState *state = g_rails_state;
    if (!state) return;

    int field_idx = -1;
    for (int i = 0; i < NUM_FIELDS; i++) {
        if (state->field_icon_layers[i] == layer) {
            field_idx = i;
            break;
        }
    }

    if (field_idx < 0) return;

    switch (state->config.fields[field_idx]) {
        case FIELD_STEPS:
            if (state->step_icon) {
                gdraw_command_image_recolor(state->step_icon,
                                            state->current_theme_palette.fgColor,
                                            state->current_theme_palette.fgColor);
                gdraw_command_image_draw(ctx, state->step_icon, GPoint(0, 0));
            }
            break;
        case FIELD_HEART_RATE:
            if (state->heart_icon) {
                gdraw_command_image_recolor(state->heart_icon,
                                            state->current_theme_palette.accentColor,
                                            state->current_theme_palette.accentColor);
                gdraw_command_image_draw(ctx, state->heart_icon, GPoint(0, 0));
            }
            break;
        case FIELD_BATTERY:
            if (state->battery_icon) {
                gdraw_command_image_recolor(state->battery_icon,
                                            state->current_theme_palette.gaugeColor,
                                            state->current_theme_palette.gaugeColor);
                gdraw_command_image_draw(ctx, state->battery_icon, GPoint(0, 0));
            }
            break;
        case FIELD_CALORIES:
            if (state->calorie_icon) {
                gdraw_command_image_recolor(state->calorie_icon,
                                            state->current_theme_palette.fgColor,
                                            state->current_theme_palette.fgColor);
                gdraw_command_image_draw(ctx, state->calorie_icon, GPoint(0, 0));
            }
            break;
        case FIELD_DISTANCE:
            if (state->distance_icon) {
                gdraw_command_image_recolor(state->distance_icon,
                                            state->current_theme_palette.fgColor,
                                            state->current_theme_palette.fgColor);
                gdraw_command_image_draw(ctx, state->distance_icon, GPoint(0, 0));
            }
            break;
        case FIELD_WEATHER:
            if (state->current_weather_icon) {
                gdraw_command_image_recolor(state->current_weather_icon,
                                            state->current_theme_palette.accentColor,
                                            state->current_theme_palette.fgColor);
                gdraw_command_image_draw(ctx, state->current_weather_icon, GPoint(0, 0));
            }
            break;
        default:
            break;
    }
}

// ========================
// Helper functions
// ========================

void rails_redraw(RailsState *state) {
    layer_mark_dirty(state->root_layer);
}

// ========================
// Click config
// ========================

static void rails_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    (void)recognizer;
    RailsState *state = (RailsState *)context;
    rails_redraw(state);
}

static void rails_select_long_down_handler(ClickRecognizerRef recognizer, void *context) {
    (void)recognizer;
    RailsState *state = (RailsState *)context;
    config_cycle_theme(state);
    rails_redraw(state);
}

static void rails_select_long_up_handler(ClickRecognizerRef recognizer, void *context) {
    (void)recognizer;
    (void)context;
}

static void rails_down_click_handler(ClickRecognizerRef recognizer, void *context) {
    (void)recognizer;
    RailsState *state = (RailsState *)context;
    state->config.theme = (ThemeId)((state->config.theme + 1) % THEME_LAST);
    config_save(state);
    display_set_theme(state, state->config.theme);
    rails_redraw(state);
}

static void rails_up_click_handler(ClickRecognizerRef recognizer, void *context) {
    (void)recognizer;
    RailsState *state = (RailsState *)context;
    state->config.theme = (ThemeId)((state->config.theme - 1 + 6) % THEME_LAST);
    config_save(state);
    display_set_theme(state, state->config.theme);
    rails_redraw(state);
}

static void rails_click_config_provider(void *context) {
    RailsState *state = (RailsState *)context;
    window_single_click_subscribe(BUTTON_ID_SELECT, rails_select_click_handler);
    window_set_click_context(BUTTON_ID_SELECT, state);
    window_long_click_subscribe(BUTTON_ID_SELECT, 500, rails_select_long_down_handler, rails_select_long_up_handler);
    window_set_click_context(BUTTON_ID_SELECT, state);
    window_single_click_subscribe(BUTTON_ID_UP, rails_up_click_handler);
    window_set_click_context(BUTTON_ID_UP, state);
    window_single_click_subscribe(BUTTON_ID_DOWN, rails_down_click_handler);
    window_set_click_context(BUTTON_ID_DOWN, state);
}

// ========================
// Window handlers (forward declarations)
// ========================
static void rails_window_unload(Window *window);

static void rails_window_load(Window *window) {
    RailsState *state = (RailsState *)window_get_user_data(window);
    Layer *window_layer = window_get_root_layer(window);

    state->root_layer = window_layer;

    // Status bar disabled - we have our own time display
    // if (state->config.show_status_bar) {
    //     state->status_bar = status_bar_layer_create();
    //     status_bar_layer_set_colors(state->status_bar,
    //                                 state->current_theme_palette.bgColor,
    //                                 state->current_theme_palette.fgColor);
    //     status_bar_layer_set_separator_mode(state->status_bar,
    //                                         StatusBarLayerSeparatorModeNone);
    //     layer_add_child(window_layer, status_bar_layer_get_layer(state->status_bar));
    // }
    state->status_bar = NULL;

    // Create time layer - full screen for fctx (it uses absolute positioning)
    GRect time_rect = layer_get_bounds(window_layer);
    state->time_layer = layer_create(time_rect);
    layer_set_update_proc(state->time_layer, time_layer_draw);
    layer_add_child(window_layer, state->time_layer);

    // Create date layer - custom layer with icon background (TimeStyle-inspired)
    // Horizontal layout: "Sun [10] May" - centered on screen
    // Positioned lower to avoid overlap with bottom row fields
    GRect date_rect = GRect(RAILS_DISPLAY_WIDTH / 2 - 50, 192, 100, 30);
    state->date_layer = layer_create(date_rect);
    layer_set_update_proc(state->date_layer, date_layer_draw);
    layer_add_child(window_layer, state->date_layer);

    // Create field layers (8 slots)
    for (int i = 0; i < NUM_FIELDS; i++) {
        GRect field_rect = GRect(field_positions[i].x, field_positions[i].y,
                                 FIELD_SLOT_W, FIELD_SLOT_H);
        state->field_layers[i] = text_layer_create(field_rect);
        text_layer_set_text_alignment(state->field_layers[i], GTextAlignmentCenter);

        // Top row (0-2) and bottom row (3-5) all get same font
        text_layer_set_font(state->field_layers[i],
                            fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

        text_layer_set_text_color(state->field_layers[i], state->current_theme_palette.fgColor);
        text_layer_set_background_color(state->field_layers[i], state->current_theme_palette.bgColor);
        layer_add_child(window_layer, text_layer_get_layer(state->field_layers[i]));
    }

    // Create gauge layers (2) - SDK 4: use layer_create, store state in global
    for (int i = 0; i < 2; i++) {
        Layer *gauge_layer = layer_create(gauge_rects[i]);
        layer_set_update_proc(gauge_layer, (LayerUpdateProc)gauges_draw);
        state->gauge_layers[i] = gauge_layer;
        layer_add_child(window_layer, gauge_layer);
    }

    // Create status icon layer at top - bluetooth icon
    int icon_y = 5;  // Top of screen
    int center_x = RAILS_DISPLAY_WIDTH / 2;

    // Bluetooth icon (centered) - 20x20 filled PDC icon for light/dark theme support
    Layer *phone_layer = layer_create(GRect(center_x - 10, icon_y, 20, 20));
    layer_set_update_proc(phone_layer, draw_phone_icon);
    layer_add_child(window_layer, phone_layer);
    state->phone_icon_layer = (BitmapLayer *)phone_layer;  // Store as BitmapLayer* for compatibility

    // Load font resources - TimeStyle's beautiful vector fonts
    state->leco_font = ffont_create_from_resource(RESOURCE_ID_LECO_FONT);
    state->avenir_regular_font = ffont_create_from_resource(RESOURCE_ID_AVENIR_REGULAR_FONT);
    state->avenir_bold_font = ffont_create_from_resource(RESOURCE_ID_AVENIR_BOLD_FONT);

    // Load icon resources - PDC vectors and PNG bitmaps
    state->step_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_STEP_ICON);
    state->heart_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_HEART_ICON);
    state->battery_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_ICON);
    state->battery_charging_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_BATTERY_CHARGING_ICON);
    state->date_bg_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_DATE_BG_LG_ICON);
    state->calorie_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_CALORIE_ICON);
    state->distance_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_DISTANCE_ICON);
    state->bluetooth_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON);

    // Load weather icons - PDC vectors from TimeStyle
    state->weather_clear_day_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLEAR_DAY);
    state->weather_clear_night_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLEAR_NIGHT);
    state->weather_cloudy_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_CLOUDY);
    state->weather_partly_cloudy_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_PARTLY_CLOUDY);
    state->weather_partly_cloudy_night_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_PARTLY_CLOUDY_NIGHT);
    state->weather_rain_light_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_RAIN_LIGHT);
    state->weather_rain_heavy_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_RAIN_HEAVY);
    state->weather_snow_light_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_SNOW_LIGHT);
    state->weather_snow_heavy_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_SNOW_HEAVY);
    state->weather_thunderstorm_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_THUNDERSTORM);
    state->weather_rain_and_snow_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_RAIN_AND_SNOW);
    state->weather_generic_icon = gdraw_command_image_create_with_resource(RESOURCE_ID_WEATHER_GENERIC);
    state->current_weather_icon = state->weather_generic_icon;  // Default to generic

    // Create field icon layers - use custom draw layers for better rendering
    for (int i = 0; i < NUM_FIELDS; i++) {
        // Icons positioned to the left of each field value, sized 25x25 to match icon PDCs
        // Field text is centered in a 35x22 rect
        // Icon has 2px gap to its text, vertically centered
        int icon_x = field_positions[i].x - 27;  // 2px gap + 25px icon = 27px to the left
        int icon_y = field_positions[i].y - 2;   // Center 25px icon with 22px field height
        GRect icon_rect = GRect(icon_x, icon_y, 25, 25);
        state->field_icon_layers[i] = layer_create(icon_rect);
        layer_set_update_proc(state->field_icon_layers[i], field_icon_draw);
        layer_add_child(window_layer, state->field_icon_layers[i]);
    }

    // Apply theme to layers now that they exist
    display_set_theme(state, state->config.theme);

    // Initialize time and date display
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);
    display_update_time(state, tick_time, HOUR_UNIT | MINUTE_UNIT);
    display_update_date(state, tick_time);

    // Initialize health data fields
    fields_update_health(state);
    fields_update_heart_rate(state);

    // Initialize battery and phone icons
    // Battery icon removed - battery is now only a field
    icons_update_phone(state, state->phone_connected);

    // Request initial weather update
    weather_request_update(state);

}

static void rails_window_unload(Window *window) {
    RailsState *state = (RailsState *)window_get_user_data(window);

    layer_destroy(state->time_layer);
    layer_destroy(state->date_layer);

    for (int i = 0; i < NUM_FIELDS; i++) {
        text_layer_destroy(state->field_layers[i]);
        layer_destroy(state->field_icon_layers[i]);
    }
    for (int i = 0; i < 2; i++) {
        layer_destroy(state->gauge_layers[i]);
    }
    // Destroy status icon layers
    if (state->phone_icon_layer) bitmap_layer_destroy(state->phone_icon_layer);

    // Free fonts
    if (state->leco_font) ffont_destroy(state->leco_font);
    if (state->avenir_regular_font) ffont_destroy(state->avenir_regular_font);
    if (state->avenir_bold_font) ffont_destroy(state->avenir_bold_font);

    // Free PDC vector icons
    if (state->step_icon) gdraw_command_image_destroy(state->step_icon);
    if (state->heart_icon) gdraw_command_image_destroy(state->heart_icon);
    if (state->battery_icon) gdraw_command_image_destroy(state->battery_icon);
    if (state->battery_charging_icon) gdraw_command_image_destroy(state->battery_charging_icon);
    if (state->date_bg_icon) gdraw_command_image_destroy(state->date_bg_icon);
    if (state->calorie_icon) gdraw_command_image_destroy(state->calorie_icon);
    if (state->distance_icon) gdraw_command_image_destroy(state->distance_icon);
    if (state->bluetooth_icon) gdraw_command_image_destroy(state->bluetooth_icon);

    // Free weather icons
    if (state->weather_clear_day_icon) gdraw_command_image_destroy(state->weather_clear_day_icon);
    if (state->weather_clear_night_icon) gdraw_command_image_destroy(state->weather_clear_night_icon);
    if (state->weather_cloudy_icon) gdraw_command_image_destroy(state->weather_cloudy_icon);
    if (state->weather_partly_cloudy_icon) gdraw_command_image_destroy(state->weather_partly_cloudy_icon);
    if (state->weather_partly_cloudy_night_icon) gdraw_command_image_destroy(state->weather_partly_cloudy_night_icon);
    if (state->weather_rain_light_icon) gdraw_command_image_destroy(state->weather_rain_light_icon);
    if (state->weather_rain_heavy_icon) gdraw_command_image_destroy(state->weather_rain_heavy_icon);
    if (state->weather_snow_light_icon) gdraw_command_image_destroy(state->weather_snow_light_icon);
    if (state->weather_snow_heavy_icon) gdraw_command_image_destroy(state->weather_snow_heavy_icon);
    if (state->weather_thunderstorm_icon) gdraw_command_image_destroy(state->weather_thunderstorm_icon);
    if (state->weather_rain_and_snow_icon) gdraw_command_image_destroy(state->weather_rain_and_snow_icon);
    if (state->weather_generic_icon) gdraw_command_image_destroy(state->weather_generic_icon);

    // Status bar disabled
    // if (state->status_bar) {
    //     status_bar_layer_destroy(state->status_bar);
    // }
}

// ========================
// Tick handler - SDK 4: no context param
// ========================

static void rails_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    Window *window = window_stack_get_top_window();
    if (!window) return;
    RailsState *state = (RailsState *)window_get_user_data(window);
    if (!state) return;

    display_update_time(state, tick_time, units_changed);
    display_update_date(state, tick_time);

    if (units_changed & (MINUTE_UNIT | HOUR_UNIT | DAY_UNIT)) {
        fields_update_health(state);
        if (units_changed & DAY_UNIT) {
            layer_mark_dirty(state->gauge_layers[0]);
            layer_mark_dirty(state->gauge_layers[1]);
            for (int i = 0; i < NUM_FIELDS; i++) {
                layer_mark_dirty(state->field_icon_layers[i]);
            }
        }
    }

    // Heart rate updates are handled by the health event handler, not tick timer

    if (units_changed & MINUTE_UNIT) {
        if (tick_time->tm_min % 30 == 0) {
            weather_request_update(state);
        }
    }

    if (units_changed & HOUR_UNIT) {
        state->battery = battery_state_service_peek();
        // Battery icon removed - battery is now only a field
    }

    // Time and date layers are now marked dirty inside display_update_time/date
    // only when values actually change

    // Only mark gauge layers dirty when health data might have changed
    if (units_changed & (MINUTE_UNIT | HOUR_UNIT | DAY_UNIT)) {
        for (int i = 0; i < 2; i++) {
            if (state->gauge_layers[i]) {
                layer_mark_dirty(state->gauge_layers[i]);
            }
        }

        // Mark field layers dirty only when data might have changed
        for (int i = 0; i < NUM_FIELDS; i++) {
            if (state->field_layers[i]) {
                text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
            }
        }
    }
}

// ========================
// App message handlers - SDK 4: uses tuple->value->int32, AppMessageResult
// ========================

static void rails_inbox_received_callback(DictionaryIterator *iter, void *context) {
    RailsState *state = (RailsState *)context;
    if (!state) return;

    Tuple *tuple;

    // Check for weather data first
    tuple = dict_find(iter, MESSAGE_KEY_WeatherTemperature);
    if (tuple || dict_find(iter, MESSAGE_KEY_WeatherCondition)) {
        weather_parse_from_dict(iter, state);
        return;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_THEME);
    if (tuple) {
        state->config.theme = (ThemeId)tuple->value->int32;
        display_set_theme(state, state->config.theme);
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_ACCENT_COLOR);
    if (tuple) {
        state->config.accent_color = (AccentColorId)tuple->value->int32;
        // Refresh theme palette with new accent color
        state->current_theme_palette = get_theme_palette(state->config.theme, state->config.accent_color);
        display_set_theme(state, state->config.theme);
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_FIELD_1);
    if (tuple) state->config.fields[0] = (FieldTypeId)tuple->value->int32;

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_FIELD_2);
    if (tuple) state->config.fields[1] = (FieldTypeId)tuple->value->int32;

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_FIELD_3);
    if (tuple) state->config.fields[2] = (FieldTypeId)tuple->value->int32;

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_FIELD_4);
    if (tuple) state->config.fields[3] = (FieldTypeId)tuple->value->int32;

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_FIELD_5);
    if (tuple) state->config.fields[4] = (FieldTypeId)tuple->value->int32;

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_FIELD_6);
    if (tuple) state->config.fields[5] = (FieldTypeId)tuple->value->int32;

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_TIME_FORMAT);
    if (tuple) {
        state->config.time_format = (TimeFormat)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_TEMP_UNIT);
    if (tuple) {
        state->config.temp_unit = (TempUnit)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_DIST_UNIT);
    if (tuple) {
        state->config.dist_unit = (DistUnit)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_GAUGE_1);
    if (tuple) {
        state->config.gauge_1 = (GaugeTypeId)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_GAUGE_2);
    if (tuple) {
        state->config.gauge_2 = (GaugeTypeId)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_GAUGE_MAX_STEPS);
    if (tuple) {
        state->config.gauge_max.steps = (uint32_t)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_GAUGE_MAX_CALORIES);
    if (tuple) {
        state->config.gauge_max.calories = (uint32_t)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_GAUGE_MAX_DISTANCE);
    if (tuple) {
        state->config.gauge_max.distance = (uint32_t)tuple->value->int32;
    }

    tuple = dict_find(iter, MESSAGE_KEY_CONFIG_KEY_SHOW_PHONE);
    if (tuple) {
        state->config.show_phone = tuple->value->int32 != 0;
        if (state->phone_icon_layer) {
            layer_set_hidden(bitmap_layer_get_layer(state->phone_icon_layer), !state->config.show_phone);
        }
    }

    config_save(state);
    fields_update_health(state);
    fields_update_heart_rate(state);
    rails_redraw(state);

    APP_LOG(APP_LOG_LEVEL_INFO, "Config received and applied");
}

static void rails_outbox_sent_callback(DictionaryIterator *iter, void *context) {
    (void)iter;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Rails: Outbox sent");
    (void)context;
}

static void rails_outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
    (void)iter;
    APP_LOG(APP_LOG_LEVEL_ERROR, "Rails: Outbox failed (%d)", reason);
    (void)context;
}

// ========================
// Connection handler - SDK 4: no context param
// ========================

static void rails_connection_handler(bool connected) {
    Window *window = window_stack_get_top_window();
    if (window) {
        RailsState *state = (RailsState *)window_get_user_data(window);
        if (state) {
            // Only update if connection state actually changed
            if (state->phone_connected != connected) {
                state->phone_connected = connected;
                icons_update_phone(state, connected);
                rails_redraw(state);
            }
        }
    }
}

// ========================
// Battery handler - SDK 4: no context param
// ========================

static void rails_battery_handler(BatteryChargeState bat) {
    Window *window = window_stack_get_top_window();
    if (window) {
        RailsState *state = (RailsState *)window_get_user_data(window);
        if (state) {
            // Only update if battery percentage actually changed
            if (state->battery.charge_percent != bat.charge_percent ||
                state->battery.is_charging != bat.is_charging ||
                state->battery.is_plugged != bat.is_plugged) {
                state->battery = bat;

                // Update battery field if it's configured
                fields_update_health(state);

                rails_redraw(state);
            }
        }
    }
}

// ========================
// Health event handler - SDK 4: HealthEventType enum
// ========================

static void rails_health_event_handler(HealthEventType event, void *context) {
    RailsState *state = (RailsState *)context;

    if (event == HealthEventHeartRateUpdate) {
        HealthValue hr = health_service_peek_current_value(HealthMetricHeartRateBPM);
        if (hr != 0) {
            // Only update if heart rate actually changed
            if (state->health.heart_rate != (uint16_t)hr) {
                state->health.heart_rate = (uint16_t)hr;
                state->health.hr_available = true;
                for (int i = 0; i < NUM_FIELDS; i++) {
                    if (state->config.fields[i] == FIELD_HEART_RATE) {
                        snprintf(state->field_buffers[i], sizeof(state->field_buffers[i]),
                                 "%hu", (unsigned short)hr);
                        text_layer_set_text(state->field_layers[i], state->field_buffers[i]);
                    }
                }
                rails_redraw(state);
            }
        }
    }
}

// ========================
// Init / Deinit
// ========================

void rails_init(RailsState *state) {
    memset(state, 0, sizeof(RailsState));
    g_rails_state = state;  // Set global for layer callbacks

    state->window = window_create();
    window_set_background_color(state->window, GColorBlack);
    window_set_user_data(state->window, state);
    window_set_window_handlers(state->window, (WindowHandlers) {
        .load = rails_window_load,
        .unload = rails_window_unload,
    });

    config_load(state);
    state->current_theme_palette = get_theme_palette(state->config.theme, state->config.accent_color);
    state->battery = battery_state_service_peek();
    state->phone_connected = connection_service_peek_pebble_app_connection();

    // Subscribe to tick timer - SDK 4: no context param
    // Only subscribe to MINUTE_UNIT and above to reduce battery consumption
    // Heart rate updates are handled by the health event handler
    tick_timer_service_subscribe(MINUTE_UNIT | HOUR_UNIT | DAY_UNIT,
                                  rails_tick_handler);

    // Click config with context
    window_set_click_config_provider_with_context(state->window,
                                                   rails_click_config_provider,
                                                   state);

    // App message - SDK 4: uses app_message_inbox_size_maximum()
    app_message_set_context(state);
    app_message_register_inbox_received(rails_inbox_received_callback);
    app_message_register_outbox_sent(rails_outbox_sent_callback);
    app_message_register_outbox_failed(rails_outbox_failed_callback);
    app_message_open(app_message_inbox_size_maximum(),
                     app_message_outbox_size_maximum());

    // Connection service
    ConnectionHandlers conn_handlers = {
        .pebble_app_connection_handler = rails_connection_handler
    };
    connection_service_subscribe(conn_handlers);

    // Battery service - SDK 4: no context param
    battery_state_service_subscribe(rails_battery_handler);

    // Health service - SDK 4: uses HealthEventType enum
    health_service_set_heart_rate_sample_period(15);
    health_service_events_subscribe(rails_health_event_handler, state);

    window_stack_push(state->window, true);
    APP_LOG(APP_LOG_LEVEL_INFO, "Rails for Pebble initialized");
}

void rails_deinit(RailsState *state) {
    tick_timer_service_unsubscribe();
    connection_service_unsubscribe();
    battery_state_service_unsubscribe();
    health_service_events_unsubscribe();
    window_destroy(state->window);
}

// ========================
// Main
// ========================

static RailsState s_rails_state;

int main(void) {
    rails_init(&s_rails_state);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Rails: Event loop starting");
    app_event_loop();
    rails_deinit(&s_rails_state);
}
