/*
 * Rails for Pebble - Weather module
 *
 * Parses weather data from companion app via AppMessage.
 * Uses TimeStyle's weather API with Open-Meteo.
 * SDK 4 compatible - uses tuple->value->int32
 */

#include "rails.h"

// Weather icon codes (matching TimeStyle's weather.js)
typedef enum {
    WEATHER_ICON_CLEAR_DAY = 0,
    WEATHER_ICON_CLEAR_NIGHT = 1,
    WEATHER_ICON_CLOUDY_DAY = 2,
    WEATHER_ICON_HEAVY_RAIN = 3,
    WEATHER_ICON_HEAVY_SNOW = 4,
    WEATHER_ICON_LIGHT_RAIN = 5,
    WEATHER_ICON_LIGHT_SNOW = 6,
    WEATHER_ICON_PARTLY_CLOUDY_NIGHT = 7,
    WEATHER_ICON_PARTLY_CLOUDY = 8,
    WEATHER_ICON_RAINING_AND_SNOWING = 9,
    WEATHER_ICON_THUNDERSTORM = 10,
    WEATHER_ICON_GENERIC = 11
} WeatherIconCode;

// Map weather icon code to appropriate icon resource
static void weather_update_icon(RailsState *state, int icon_code) {
    switch (icon_code) {
        case WEATHER_ICON_CLEAR_DAY:
            state->current_weather_icon = state->weather_clear_day_icon;
            break;
        case WEATHER_ICON_CLEAR_NIGHT:
            state->current_weather_icon = state->weather_clear_night_icon;
            break;
        case WEATHER_ICON_CLOUDY_DAY:
            state->current_weather_icon = state->weather_cloudy_icon;
            break;
        case WEATHER_ICON_HEAVY_RAIN:
            state->current_weather_icon = state->weather_rain_heavy_icon;
            break;
        case WEATHER_ICON_HEAVY_SNOW:
            state->current_weather_icon = state->weather_snow_heavy_icon;
            break;
        case WEATHER_ICON_LIGHT_RAIN:
            state->current_weather_icon = state->weather_rain_light_icon;
            break;
        case WEATHER_ICON_LIGHT_SNOW:
            state->current_weather_icon = state->weather_snow_light_icon;
            break;
        case WEATHER_ICON_PARTLY_CLOUDY_NIGHT:
            state->current_weather_icon = state->weather_partly_cloudy_night_icon;
            break;
        case WEATHER_ICON_PARTLY_CLOUDY:
            state->current_weather_icon = state->weather_partly_cloudy_icon;
            break;
        case WEATHER_ICON_RAINING_AND_SNOWING:
            state->current_weather_icon = state->weather_rain_and_snow_icon;
            break;
        case WEATHER_ICON_THUNDERSTORM:
            state->current_weather_icon = state->weather_thunderstorm_icon;
            break;
        default:
            state->current_weather_icon = state->weather_generic_icon;
            break;
    }

    // Mark field icon layers dirty to redraw weather icon
    for (int i = 0; i < NUM_FIELDS; i++) {
        if (state->config.fields[i] == FIELD_WEATHER) {
            layer_mark_dirty(state->field_icon_layers[i]);
        }
    }
}

void weather_parse_from_dict(DictionaryIterator *iter, RailsState *state) {
    if (!iter) return;

    Tuple *tuple;

    tuple = dict_find(iter, MESSAGE_KEY_WeatherTemperature);
    if (tuple) {
        state->weather.temp_celsius = (int8_t)tuple->value->int32;
        APP_LOG(APP_LOG_LEVEL_INFO, "Weather temperature: %d", state->weather.temp_celsius);
    }

    tuple = dict_find(iter, MESSAGE_KEY_WeatherCondition);
    if (tuple) {
        int icon_code = (int)tuple->value->int32;
        APP_LOG(APP_LOG_LEVEL_INFO, "Weather icon code: %d", icon_code);
        weather_update_icon(state, icon_code);
    }

    // Update fields that show weather
    fields_update_health(state);
}

void weather_request_update(RailsState *state) {
    (void)state;
    DictionaryIterator *iter;
    AppMessageResult result = app_message_outbox_begin(&iter);
    if (result == APP_MSG_OK) {
        int32_t trigger = 1;
        dict_write_int(iter, MESSAGE_KEY_RequestWeatherUpdate, &trigger, sizeof(int32_t), true);
        app_message_outbox_send();
        APP_LOG(APP_LOG_LEVEL_INFO, "Requested weather update from phone");
    }
}
