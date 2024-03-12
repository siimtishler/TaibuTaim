#include "Adafruit_MAX1704X.h"
#include "gauge.h"
#include "timeutils.h"

#define DEBUG
#include "SerialDebug.h"

#define GAUGE_TIMEOUT SEC_TO_MS(5)

Adafruit_MAX17048 lipo;

boolean initGauge(){
    if(lipo.begin()){
        DBGL("Lipo inited");
        DBG("ID ");
        DBGL(lipo.getChipID());
        DBG("IC Version");
        DBGL(lipo.getICversion());
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

void gaugeTask(gauge_measurements_t* measurements){
    static uint32_t last_task_ms = 0;
    if(millis() - last_task_ms > GAUGE_TIMEOUT){

        measurements->voltage = getCellVoltage();
        measurements->percentage = getPercentage();
        measurements->chg_rate = getChgRate();

        // printGaugeMeasurements(measurements);

        last_task_ms = millis();
    }
}
