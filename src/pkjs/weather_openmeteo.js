var weatherCommon = require('./weather');

module.exports.getWeatherFromCoords = getWeatherFromCoords;

function getWeatherFromCoords(pos) {
  console.log('Getting weather from Open-Meteo');
  var lat = pos.coords.latitude;
  var lon = pos.coords.longitude;

  var url = 'https://api.open-meteo.com/v1/forecast?latitude=' +
    lat + '&longitude=' + lon +
    '&current_weather=true' +
    '&temperature_unit=celsius' +
    '&timezone=auto';

  console.log(url);

  getAndSendWeather(url);
}

function getAndSendWeather(url) {
  weatherCommon.xhrRequest(url, 'GET', function(responseText) {
    var json = JSON.parse(responseText);

    // Handle current weather
    var temperature = Math.round(json.current_weather.temperature);
    var conditionCode = json.current_weather.weathercode;
    var isNight = json.current_weather.is_day !== 1;

    console.log('Current temperature is ' + temperature);
    console.log('Current condition code is ' + conditionCode);
    console.log('Is night: ' + isNight);

    var iconToLoad = getIconForWeatherCode(conditionCode, isNight);

    var dictionary = {
      'WeatherTemperature': temperature,
      'WeatherCondition': iconToLoad
    };

    console.log(JSON.stringify(dictionary));

    weatherCommon.sendWeatherToPebble(dictionary);
  });
}

function getIconForWeatherCode(code, isNight) {
  switch (code) {
    case 0:
        return isNight ? weatherCommon.icons.CLEAR_NIGHT : weatherCommon.icons.CLEAR_DAY;
    case 1:
    case 2:
        return isNight ? weatherCommon.icons.PARTLY_CLOUDY_NIGHT : weatherCommon.icons.PARTLY_CLOUDY;
    case 3:
    case 45:
    case 48:
        return weatherCommon.icons.CLOUDY_DAY;
    case 51:
    case 53:
    case 55:
    case 61:
    case 80:
        return weatherCommon.icons.LIGHT_RAIN;
    case 63:
    case 65:
    case 81:
    case 82:
        return weatherCommon.icons.HEAVY_RAIN;
    case 56:
    case 57:
    case 66:
    case 67:
        return weatherCommon.icons.RAINING_AND_SNOWING;
    case 71:
    case 77:
    case 85:
        return weatherCommon.icons.LIGHT_SNOW;
    case 73:
    case 75:
    case 86:
        return weatherCommon.icons.HEAVY_SNOW;
    case 95:
    case 96:
    case 99:
        return weatherCommon.icons.THUNDERSTORM;
    default:
        return weatherCommon.icons.WEATHER_GENERIC;
  }
}
