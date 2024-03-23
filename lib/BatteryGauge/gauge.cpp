#include "Adafruit_MAX1704X.h"
#include "gauge.h"
#include "timeutils.h"

#define DEBUG
#include "SerialDebug.h"

#define GAUGE_TIMEOUT       SEC_TO_MS(5)

#define LIPO_SDA_PIN        7
#define LIPO_SCL_PIN        6

Adafruit_MAX17048 lipo;

boolean initGauge(){
    if(lipo.begin(&Wire, LIPO_SDA_PIN, LIPO_SCL_PIN)){
        DBGL("Lipo inited");
        // DBG("ID ");
        // DBGL(lipo.getChipID());
        // DBG("IC Version");
        // DBGL(lipo.getICversion());
        return true;
    }
    return false;
}

float getCellVoltage (){
    return lipo.cellVoltage();
}

float getPercentage(){
    return lipo.cellPercent();
}

float getChgRate(){
    return lipo.chargeRate();
}

void printGaugeMeasurements(gauge_measurements_t* measurements){
    DBG("Voltage: ");
    DBG(measurements->voltage);DBGL("V");
    DBG("Percentage: ");
    DBG(measurements->percentage);DBGL("%");
    DBG("Charge rate: ");
    DBG(measurements->chg_rate);DBGL("%");
}

gauge_measurements_t getGaugeData() {
    gauge_measurements_t measurements = {0};

    measurements.percentage = getPercentage();
    measurements.voltage = getCellVoltage();
    measurements.chg_rate = getChgRate();

#ifdef DEBUG
    printGaugeMeasurements(&measurements);
#endif

    return measurements;
}

void gaugeTask(gauge_measurements_t* measurements){
    static uint32_t last_task_ms = 0;
    if(millis() - last_task_ms > GAUGE_TIMEOUT){

        measurements->voltage = getCellVoltage();
        measurements->percentage = getPercentage();
        measurements->chg_rate = getChgRate();

        printGaugeMeasurements(measurements);

        last_task_ms = millis();
    }
}
