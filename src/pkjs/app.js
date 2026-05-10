/**
 * Rails for Pebble - PebbleKit JavaScript
 * Handles configuration page and communication with the watch
 */

// Import the Clay config library (if using Clay - we're using custom HTML instead)
// var Clay = require('pebble-clay');
// var clayConfig = require('./config.json');
// var clay = new Clay(clayConfig);

var configUrl = 'https://raw.githack.com/YOUR_USERNAME/rails-pebble/main/src/pkjs/config.html';

// For local development, use file:// URL:
// Note: In production, you should host this on GitHub Pages or similar
var localConfigUrl = 'file://' + __dirname + '/config.html';

// Message keys - must match appinfo.json
var MESSAGE_KEYS = {
    CONFIG_KEY_THEME: 0,
    CONFIG_KEY_FIELD_1: 1,
    CONFIG_KEY_FIELD_2: 2,
    CONFIG_KEY_FIELD_3: 3,
    CONFIG_KEY_FIELD_4: 4,
    CONFIG_KEY_FIELD_5: 5,
    CONFIG_KEY_FIELD_6: 6,
    CONFIG_KEY_FIELD_7: 7,
    CONFIG_KEY_FIELD_8: 8,
    CONFIG_KEY_TIME_FORMAT: 9,
    CONFIG_KEY_GAUGE_1: 10,
    CONFIG_KEY_GAUGE_2: 11,
    CONFIG_KEY_SHOW_PHONE: 12,
};

// Default configuration
var defaultConfig = {
    theme: 0,
    field1: 1,
    field2: 2,
    field3: 3,
    field4: 4,
    field5: 5,
    field6: 6,
    field7: 9,
    field8: 10,
    gauge1: 1,
    gauge2: 2,
    timeFormat: 0,
    showPhone: 1,
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
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_1] = config.field1;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_2] = config.field2;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_3] = config.field3;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_4] = config.field4;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_5] = config.field5;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_6] = config.field6;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_7] = config.field7;
    dict[MESSAGE_KEYS.CONFIG_KEY_FIELD_8] = config.field8;
    dict[MESSAGE_KEYS.CONFIG_KEY_TIME_FORMAT] = config.timeFormat;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_1] = config.gauge1;
    dict[MESSAGE_KEYS.CONFIG_KEY_GAUGE_2] = config.gauge2;
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
});

// Listen for configuration page open
Pebble.addEventListener('showConfiguration', function(e) {
    console.log('Showing configuration page');

    var config = loadConfig();

    // Build configuration URL with current settings
    var url = localConfigUrl + '?' +
        'theme=' + config.theme +
        '&field1=' + config.field1 +
        '&field2=' + config.field2 +
        '&field3=' + config.field3 +
        '&field4=' + config.field4 +
        '&field5=' + config.field5 +
        '&field6=' + config.field6 +
        '&field7=' + config.field7 +
        '&field8=' + config.field8 +
        '&gauge1=' + config.gauge1 +
        '&gauge2=' + config.gauge2 +
        '&timeFormat=' + config.timeFormat +
        '&dateFormat=' + config.dateFormat +
        '&showCompass=' + config.showCompass +
        '&showWeather=' + config.showWeather +
        '&showBattery=' + config.showBattery +
        '&showPhone=' + config.showPhone +
        '&showNotifications=' + config.showNotifications +
        '&showStatusBar=' + config.showStatusBar;

    console.log('Opening config URL:', url);
    Pebble.openURL(url);
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

    // Handle any messages from the watch here
    // For example, weather requests, etc.
});

console.log('Rails PebbleKit JS loaded');
