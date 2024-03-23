#include <Arduino.h>
#include "lightsensor.h"
#include "gauge.h"
#include "soilsensor.h"
#include "humiditysensor.h"
#include "driver/temp_sensor.h"

#include "sensorstask.h"


#include "bleserial.h"
#include "RTDB.h"

#include "timeutils.h"

#define DEBUG
#include "SerialDebug.h"


void initTempSensor(){
    temp_sensor_config_t temp_sensor = TSENS_CONFIG_DEFAULT();
    temp_sensor.dac_offset = TSENS_DAC_L2;  // TSENS_DAC_L2 is default; L4(-40°C ~ 20°C), L2(-10°C ~ 80°C), L1(20°C ~ 100°C), L0(50°C ~ 125°C)
    temp_sensor_set_config(temp_sensor);
    temp_sensor_start();
}

light_sens_measurements_t light_measurements = {0};
gauge_measurements_t gauge_measurements = {0};

#include "Wire.h"

void setup()
{

	Serial.begin(115200);
	DBGL("*******/TERE TAIBUTAIM\\*******");
	delay(10);

	// initSoilSensor();
	
	
	// initBleSerial();
	ConnectWifi();
	ConnectFirebase();
}
void loop()
{
	sensorsTask();
	// sensors_event_t humidity, temp;
	// shtc3.getEvent(&humidity, &temp);
	// DBG("Temp: "); DBGL(temp.temperature);
	// DBG("Humidity: "); DBGL(humidity.relative_humidity);

	// float tempr = 0;
	// temp_sensor_read_celsius(&tempr);
	// DBG("Temp from internal "); DBGL(tempr);
	// Wire.end();
	// delay(10);

	// initHumiditySensor();

	// measureHumidityAndTemp();
	// getLastMeasuredHumidity();

	// Wire.end();
	// delay(10);
	// Wire.begin(7,6);

	// if(initLightSensor()) {
	// 	DBGL(getLux());
	// }
	// else {
	// 	DBGL("Putsis");
	// }
	// delay(5000);

	// measureSoil();

	// lightSensorTask(&light_measurements);
	// gaugeTask(&gauge_measurements);
	// sendLightSensorStatus(light_measurements);

	// sendBatteryStatus(gauge_measurements);
	// bleSerialTask();
}

// #include "Adafruit_VEML7700.h"

// Adafruit_VEML7700 veml = Adafruit_VEML7700();

// void setup() {
//   Serial.begin(115200);
//   while (!Serial) { delay(10); }
//   Serial.println("Adafruit VEML7700 Test");

//   if (!veml.begin()) {
//     Serial.println("Sensor not found");
//     while (1);
//   }
//   Serial.println("Sensor found");

//   // == OPTIONAL =====
//   // Can set non-default gain and integration time to
//   // adjust for different lighting conditions.
//   // =================
//   // veml.setGain(VEML7700_GAIN_1_8);
//   // veml.setIntegrationTime(VEML7700_IT_100MS);

//   Serial.print(F("Gain: "));
//   switch (veml.getGain()) {
//     case VEML7700_GAIN_1: Serial.println("1"); break;
//     case VEML7700_GAIN_2: Serial.println("2"); break;
//     case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
//     case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
//   }

//   Serial.print(F("Integration Time (ms): "));
//   switch (veml.getIntegrationTime()) {
//     case VEML7700_IT_25MS: Serial.println("25"); break;
//     case VEML7700_IT_50MS: Serial.println("50"); break;
//     case VEML7700_IT_100MS: Serial.println("100"); break;
//     case VEML7700_IT_200MS: Serial.println("200"); break;
//     case VEML7700_IT_400MS: Serial.println("400"); break;
//     case VEML7700_IT_800MS: Serial.println("800"); break;
//   }

//   veml.setLowThreshold(10000);
//   veml.setHighThreshold(20000);
//   veml.interruptEnable(false);
// }

// void loop() {
//   Serial.print("raw ALS: "); Serial.println(veml.readALS());
//   Serial.print("raw white: "); Serial.println(veml.readWhite());
//   Serial.print("lux: "); Serial.println(veml.readLux(VEML_LUX_AUTO));

//   uint16_t irq = veml.interruptStatus();
//   if (irq & VEML7700_INTERRUPT_LOW) {
//     Serial.println("** Low threshold");
//   }
//   if (irq & VEML7700_INTERRUPT_HIGH) {
//     Serial.println("** High threshold");
//   }
//   delay(500);
// }