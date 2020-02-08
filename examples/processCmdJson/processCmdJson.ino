/*
Library for reading DDS238-4 W Wifi Smart meter (SM).
Reading via Hardware Serial
2020 (development with PlatformIO IDE for VSCode & esp8266 core)

MIT License

Copyright (c) 2020 Rodrigo González Zárate

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <Arduino.h>

#include <JSmartMeter238.h>   //import JSmartMeter238 library
#include <SmartMeter238.h>    //import SmartMeter238 library

//-----------------------------------------------------------------------

// Only for debug purposes
HardwareSerial &meter = Serial;
HardwareSerial &debug = Serial1;

//-----------------------------------------------------------------------

char payloadBuffer[JSM_JSON_BUFFER];   // Buffer Json data

#ifdef SM_ENABLE_DEBUG
SmartMeter238 sm(meter, debug);   // config SmartMeter238 with debug
JSmartMeter238 jsm(sm, debug);    // config JSmartMeter238 with debug
#else
SmartMeter238 sm(meter);   // config SmartMeter238
JSmartMeter238 jsm(sm);    // config JSmartMeter238
#endif

// Data storage
SmartMeter238::smartMeterData smData;

void processCmdJson(const char *json) {
    debug.println(json);

    unsigned int len = jsm.processCmdJson(payloadBuffer, json, strlen(json));

    if (len > 0) {
        debug.println(payloadBuffer);
    } else {
        debug.println("Fail Json");
    }

    debug.println();
}

void setup() {
    debug.begin(9600);   // Start Serial Debug

    sm.begin();   // initialize SmartMeter238 communication

    jsm.begin(smData);   // initialize JSmartMeter238 communication

    jsm.setJsonPretty(true);   // For use setJsonPretty you must need increase json buffer JSM_JSON_BUFFER = 608, this is for test only
}

void loop() {
    debug.println();

    debug.println("-----------------------------------------------------------------------");

    processCmdJson("{\"cmd\":\"getPowerCutData\"}");

    processCmdJson("{\"cmd\":\"getMeasurementData\"}");

    processCmdJson("{\"cmd\":\"getLimitData\"}");

    processCmdJson("{\"cmd\":\"getPurchaseData\"}");

    processCmdJson("{\"cmd\":\"getPowerCompanyData\"}");

    processCmdJson("{\"cmd\":\"setLimitsData\",\"data\":{\"maxCurrentLimit\":50.00,\"maxVoltageLimit\":270,\"minVoltageLimit\":175}}");

    processCmdJson("{\"cmd\":\"setPurchaseData\",\"data\":{\"energyPurchase\":1000.00,\"energyPurchaseAlarm\":500.00,\"energyPurchaseStatus\":true}}");

    processCmdJson("{\"cmd\":\"setPowerCutData\",\"data\":{\"powerCut\":true}}");

    processCmdJson("{\"cmd\":\"setDelay\",\"data\":{\"delaySetPowerCut\":true,\"delay\":60}}");

    processCmdJson("{\"cmd\":\"setReset\"}");

    processCmdJson("{\"cmd\":\"setPowerCompanyData\",\"data\":{\"startingKWh\":20998.99,\"priceKWh\":120.26}}");

#ifdef SM_ENABLE_RAW_TEST_MSG
    // You must define SM_ENABLE_RAW_TEST_MSG, this functions is only for debug and development
    processCmdJson("{\"cmd\":\"getRawMessage\"}");

    processCmdJson("{\"cmd\":\"sendRawMessage\",\"data\":{\"hex\":\"48:06:02:01:0A:5B\"}}");
#endif

    debug.println("-----------------------------------------------------------------------");

    delay(3000);
}