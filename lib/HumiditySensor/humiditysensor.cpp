#include "humiditysensor.h"
#include "Adafruit_SHTC3.h"

// #define DEBUG
#include "SerialDebug.h"

#define HUMIDITY_SDA_PIN        6
#define HUMIDITY_SCL_PIN        7

Adafruit_SHTC3 humiditysensor = Adafruit_SHTC3();
sensors_event_t humidity, temp;

RTC_DATA_ATTR boolean sleeping = false;

boolean initHumiditySensor() {

    for (int i = 0; i < 10; i++)
    {
        if(humiditysensor.begin(&Wire, HUMIDITY_SDA_PIN, HUMIDITY_SCL_PIN, false)) {
            DBGL("Humidity sensor init success");
            delay(10);
            return true;
        }
    }
    return false;
}   

void humiditySensorSleepEnable(boolean enable) {
    humiditysensor.sleep(enable);
}

boolean powerOnHumiditySensor() {

    for (int i = 0; i < 10; i++)
    {
        if(humiditysensor.begin(&Wire, HUMIDITY_SDA_PIN, HUMIDITY_SCL_PIN, true)) {
            DBGL("Humidity sensor power on success");
            // humiditysensor.lowPowerMode(true);
            delay(10);
            return true;
        }
    }
    DBGL("Moisture sensor power on failed");
    return false;
}   

void printHumidityAndTemperatureMeasurements(humidity_measurements_t* measurements){
    DBG("Humidity: ");
    DBG(measurements->humidity);DBGL("%");
    DBG("Temperature: ");
    DBG(measurements->temperature);DBGL("C");
}


humidity_measurements_t getHumidityAndTemperatureMeasurements() {
    humidity_measurements_t measurements = {0}; 

    measureHumidityAndTemp();

    measurements.humidity = getLastMeasuredHumidity();
    measurements.temperature = getLastMeasuredTemperature();

#ifdef DEBUG
    printHumidityAndTemperatureMeasurements(&measurements);
#endif

    return measurements;
}

void measureHumidityAndTemp() {
    humiditysensor.getEvent(&humidity, &temp);
}

float getLastMeasuredTemperature() {
    return temp.temperature;
}

float getLastMeasuredHumidity() {
    return humidity.relative_humidity;
}