import requests
from dotenv import load_dotenv
import os
from geopy.geocoders import Nominatim
import json
import time


def openweathermap_forecasts_rain(lat, lon) -> bool:
    '''
    Check OpenWeatherMap API forecast for rain in the next 24 hours.

    Args:
        lat (float): Latitude of the location
        lon (float): Longitude of the location

    Returns:
        bool: True if rain is forecast in the next 24 hours, False otherwise
    '''
    OPENWEATHERMAP_API_KEY = os.getenv('OPENWEATHERMAP_API_KEY')

    # Go to https://home.openweathermap.org/myservices > view > scroll down to Free Tier
    url = f'https://api.openweathermap.org/data/2.5/forecast?lat={lat}&lon={lon}&appid={OPENWEATHERMAP_API_KEY}'
    response = requests.get(url, params=None, headers=None, timeout=None)
    forecast_data = response.json()
    print(f'forecast: {json.dumps(forecast_data, indent=2)}')
    # Get current time in Unix timestamp
    current_time = int(time.time())
    # 24 hours in seconds
    day = 60 * 60 * 24

    for period in forecast_data['list']:
        # Skip if forecast is beyond 24 hours
        if period['dt'] > current_time + day:
            continue

        # Else check rain conditions
        weather_main = period['weather'][0]['main']
        pop = period['pop']
        has_rain = 'rain' in period

        print(f"Checking period {period['dt_txt']}: "
              f"Weather={weather_main}, "
              f"Probability={pop}, "
              f"Has rain data={has_rain}")
        if weather_main == 'Rain' and pop > 0.55 and has_rain:
            return True

        return False

    # weather request
    # url = f'https://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&exclude={part}&appid={OPENWEATHERMAP_API_KEY}'
    # map request
    # layer = "precipitation_new"
    # z = 0
    # x = 1
    # y = 1
    # url = f'https://tile.openweathermap.org/map/{layer}/{z}/{x}/{y}.png?appid={OPENWEATHERMAP_API_KEY}'
    # result = requests.get(url, params=None, headers=None, timeout=None)
    # print(f'result = {result}')

# OpenWeatherMap API Request
load_dotenv()

def main():
    '''
    Script to make api calls and send an alert if rain is forecast in my area.
    '''
    # Create a geocoder object - we're telling geopy which service to use
    # The user_agent helps the service identify who is making the request
    geocoder = Nominatim(user_agent="my_weather_app")

    # Convert an address to coordinates
    # The geocode function returns a location object with latitude and longitude
    location = geocoder.geocode("Costa Mesa")
    lat = location.latitude
    lon = location.longitude
    # Openweathermap API request
    if openweathermap_forecasts_rain(lat, lon):
        print('RAIN!!!!')
        
    print('No rain.')        


if __name__ == "__main__":
    main()
