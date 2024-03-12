#pragma once
#include <Arduino.h>


#ifdef DEBUG
#define DBGL(x) Serial.println(x)
#define DBG(x) Serial.print(x)
#else 
#define DBGL(x) ((void)0)
#define DBG(x) ((void)0)
#endif

