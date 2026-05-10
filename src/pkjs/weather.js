/* Weather module for Rails - using TimeStyle's approach with Open-Meteo API */

var weatherProvider = require('./weather_openmeteo');

// icon codes for sending weather icons to pebble
var WeatherIcons = {
  CLEAR_DAY           : 0,
  CLEAR_NIGHT         : 1,
  CLOUDY_DAY          : 2,
  HEAVY_RAIN          : 3,
  HEAVY_SNOW          : 4,
  LIGHT_RAIN          : 5,
  LIGHT_SNOW          : 6,
  PARTLY_CLOUDY_NIGHT : 7,
  PARTLY_CLOUDY       : 8,
  RAINING_AND_SNOWING : 9,
  THUNDERSTORM        : 10,
  WEATHER_GENERIC     : 11
};

function updateWeather() {
  console.log("Getting weather update");

  navigator.geolocation.getCurrentPosition(
    function(pos) {
      weatherProvider.getWeatherFromCoords(pos);
    },
    function(err) {
      console.log('Location error: ' + err.message);
    },
    {timeout: 15000, maximumAge: 60000}
  );
}

function sendWeatherToPebble(dictionary) {
  // Send to Pebble
  Pebble.sendAppMessage(dictionary,
    function(e) {
      console.log('Weather info sent to Pebble successfully!');
    },
    function(e) {
      console.log('Error sending weather info to Pebble!');
    }
  );
}

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// exports
module.exports.icons = WeatherIcons;
module.exports.xhrRequest = xhrRequest;
module.exports.sendWeatherToPebble = sendWeatherToPebble;
module.exports.updateWeather = updateWeather;
