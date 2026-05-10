/**
 * Rails for Pebble - PebbleKit JavaScript
 * Handles configuration page and communication with the watch
 */

// Import weather module
var weather = require('./weather');

// Message keys - auto-generated from package.json
// These values MUST match build/js/message_keys.json
var MESSAGE_KEYS = {
    CONFIG_KEY_THEME: 10000,
    CONFIG_KEY_ACCENT_COLOR: 10001,
    CONFIG_KEY_FIELD_1: 10002,
    CONFIG_KEY_FIELD_2: 10003,
    CONFIG_KEY_FIELD_3: 10004,
    CONFIG_KEY_FIELD_4: 10005,
    CONFIG_KEY_FIELD_5: 10006,
    CONFIG_KEY_FIELD_6: 10007,
    CONFIG_KEY_TIME_FORMAT: 10008,
    CONFIG_KEY_TEMP_UNIT: 10009,
    CONFIG_KEY_DIST_UNIT: 10010,
    CONFIG_KEY_GAUGE_1: 10011,
    CONFIG_KEY_GAUGE_2: 10012,
    CONFIG_KEY_GAUGE_MAX_STEPS: 10013,
    CONFIG_KEY_GAUGE_MAX_CALORIES: 10014,
    CONFIG_KEY_GAUGE_MAX_DISTANCE: 10015,
    CONFIG_KEY_SHOW_PHONE: 10016,
};

// Default configuration
var defaultConfig = {
    theme: 1,  // THEME_LIGHT
    accentColor: 0,  // ACCENT_BLUE
    field1: 1,  // FIELD_STEPS
    field2: 2,  // FIELD_CALORIES
    field3: 3,  // FIELD_DISTANCE
    field4: 5,  // FIELD_HEART_RATE
    field5: 6,  // FIELD_WEATHER
    field6: 7,  // FIELD_BATTERY
    gauge1: 1,  // GAUGE_STEPS
    gauge2: 2,  // GAUGE_CALORIES
    gaugeMaxSteps: 0,         // 0 = auto (7-day avg + 10%)
    gaugeMaxCalories: 2000,
    gaugeMaxDistance: 8000,   // meters
    timeFormat: 0,
    tempUnit: 1,  // Fahrenheit
    distUnit: 1,  // Miles
    showPhone: 1
};

// Load config from localStorage
function loadConfig() {
    try {
        var config = JSON.parse(localStorage.getItem('railsConfig'));
        if (config) {
            console.log('Loaded config from localStorage:', JSON.stringify(config));
            return config;
        }
    } catch (e) {
        console.log('Error loading config:', e);
    }
    console.log('Using default config');
    return defaultConfig;
}

// Save config to localStorage
function saveConfig(config) {
    localStorage.setItem('railsConfig', JSON.stringify(config));
    console.log('Saved config to localStorage:', JSON.stringify(config));
}

// Send config to watch
function sendConfigToWatch(config) {
    var dict = {};
    dict[MESSAGE_KEYS.CONFIG_KEY_THEME] = config.theme;
    dict[MESSAGE_KEYS.CONFIG_KEY_ACCENT_COLOR] = config.accentColor;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_1] = config.field1;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_2] = config.field2;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_3] = config.field3;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_4] = config.field4;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_5] = config.field5;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_6] = config.field6;
    dict[MESSAGE_KEYS.CONFIG_KEY_TIME_FORMAT] = config.timeFormat;
    dict[MESSAGE_KEYS.CONFIG_KEY_TEMP_UNIT] = config.tempUnit;
    dict[MESSAGE_KEYS.CONFIG_KEY_DIST_UNIT] = config.distUnit;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_1] = config.gauge1;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_2] = config.gauge2;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_MAX_STEPS] = config.gaugeMaxSteps;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_MAX_CALORIES] = config.gaugeMaxCalories;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_MAX_DISTANCE] = config.gaugeMaxDistance;
    dict[MESSAGE_KEYS.CONFIG_KEY_SHOW_PHONE] = config.showPhone;

    console.log('Sending config to watch:', JSON.stringify(dict));

    Pebble.sendAppMessage(dict,
        function() {
            console.log('Config sent successfully');
        },
        function(e) {
            console.log('Failed to send config: ' + JSON.stringify(e));
        }
    );
}

// Listen for when the watchface is ready
Pebble.addEventListener('ready', function(e) {
    console.log('PebbleKit JS ready!');

    // Send current config to watch
    var config = loadConfig();
    sendConfigToWatch(config);

    // Get initial weather
    weather.updateWeather();
});

// Listen for configuration page open
Pebble.addEventListener('showConfiguration', function(e) {
    console.log('Showing configuration page');

    var config = loadConfig();

    // Build URL to hosted config page with current config embedded
    // Using GitHub Pages to serve the HTML
    var configUrl = 'https://superm1.github.io/rails-pebble/config.html#' +
        encodeURIComponent(JSON.stringify(config));

    console.log('Opening configuration page:', configUrl);
    Pebble.openURL(configUrl);
});

// Listen for configuration page close
Pebble.addEventListener('webviewclosed', function(e) {
    console.log('Configuration page closed');
    console.log('Response: ' + e.response);

    if (e.response) {
        try {
            var config = JSON.parse(decodeURIComponent(e.response));
            console.log('Received config:', JSON.stringify(config));

            // Save to localStorage
            saveConfig(config);

            // Send to watch
            sendConfigToWatch(config);
        } catch (error) {
            console.log('Error parsing config:', error);
        }
    } else {
        console.log('No config data received (user cancelled)');
    }
});

// Listen for messages from the watch
Pebble.addEventListener('appmessage', function(e) {
    console.log('Received message from watch:', JSON.stringify(e.payload));

    // Handle weather update requests
    if (e.payload.RequestWeatherUpdate !== undefined) {
        console.log('Watch requested weather update');
        weather.updateWeather();
    }
});

console.log('Rails PebbleKit JS loaded');
