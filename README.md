# JSmartMeter238

This library provides a convenient way (JSON) to interact with the SmartMeter238 library.

## Usage
```c++
#include <SmartMeter238.h>       //import SmartMeter238 library
#include <JSmartMeter238.h>   //import JSmartMeter238 library

char payloadBuffer[JSM_JSON_BUFFER];   // Buffer Json data

SmartMeter238 sm(Serial);     // config SmartMeter238
JSmartMeter238 jsm(sm);   // config JSmartMeter238

// Data storage
SmartMeter238::smartMeterData smData;
```
---
```c++
void setup() {
    Serial1.begin(9600);   // Start Serial Debug

    sm.begin();   // initialize SmartMeter238 communication
    jsm.begin(smData);   // initialize JSmartMeter238 communication
}
```
---
You can use command as const char and Data (json) payload:
```c++
void processCmd(const char *cmdStr, const char *json) {
    JSmartMeter238::jsonCommands cmd = jsm.resolveCommand(cmdStr);
    
    jsm.processCmd(cmd, payloadBuffer, json, strlen(json));

    Serial1.println(payloadBuffer);
}

void loop() {
    processCmd("getPowerCutData", "");
    processCmd("getMeasurementData", "");
    processCmd("getLimitData", "");
    processCmd("getPurchaseData", "");
    processCmd("getPowerCompanyData", "");
    processCmd("setLimitsData", "{\"data\":{\"maxCurrentLimit\":50,\"maxVoltageLimit\":270,\"minVoltageLimit\":175}}");
    processCmd("setPurchaseData", "{\"data\":{\"energyPurchase\":1000.00,\"energyPurchaseAlarm\":500.00,\"energyPurchaseStatus\":true}}");
    processCmd("setPowerCutData", "{\"data\":{\"powerCut\":true}}");
    processCmd("setDelay", "{\"data\":{\"delaySetPowerCut\":true,\"delay\":60}}");
    processCmd("setReset", "");
    processCmd("setPowerCompanyData", "{\"data\":{\"startingKWh\":20998.99,\"priceKWh\":120.26}}");
    
    delay(3000);
}
```
---
Or you can use command in Json payload:
```c++
void processCmdJson(const char *json) {
    unsigned int len = jsm.processCmdJson(payloadBuffer, json, strlen(json));

    if (len > 0) {
        Serial1.println(payloadBuffer);
    }
}

void loop() {
    processCmdJson("{\"cmd\":\"getPowerCutData\"}");
    processCmdJson("{\"cmd\":\"getMeasurementData\"}");
    processCmdJson("{\"cmd\":\"getLimitData\"}");
    processCmdJson("{\"cmd\":\"getPurchaseData\"}");
    processCmdJson("{\"cmd\":\"getPowerCompanyData\"}");
    processCmdJson("{\"cmd\":\"setLimitsData\",\"data\":{\"maxCurrentLimit\":50,\"maxVoltageLimit\":270,\"minVoltageLimit\":175}}");
    processCmdJson("{\"cmd\":\"setPurchaseData\",\"data\":{\"energyPurchase\":1000.00,\"energyPurchaseAlarm\":500.00,\"energyPurchaseStatus\":true}}");
    processCmdJson("{\"cmd\":\"setPowerCutData\",\"data\":{\"powerCut\":true}}");
    processCmdJson("{\"cmd\":\"setDelay\",\"data\":{\"delaySetPowerCut\":true,\"delay\":60}}");
    processCmdJson("{\"cmd\":\"setReset\"}");
    processCmdJson("{\"cmd\":\"setPowerCompanyData\",\"data\":{\"startingKWh\":20998.99,\"priceKWh\":120.26}}");

    delay(3000);
}
```

## Compatible Hardware

The library uses ESP8266 Core for interacting with the underlying network hardware. This means it Just Works with a growing number of boards and shields, including:

- ESP8266

## JSON Command

### Get power cut data
 - Command "getPowerCutData"
 - Response
```json
{
	"response": "getPowerCutData",
	"time": 15025622563,
	"data": {
		"powerCut": false,
		"powerCutDetails": "No Power Cut",
		"delay": 60,
		"delaySetPowerCut": false
	}
}
```

### Get current measurement data
 - Command "getMeasurementData"
 - Response
```json
{
	"response": "getMeasurementData",
	"time": 15025622563,
	"data": {
		"current": 1.202,
		"voltage": 222.9,
		"frequency": 49.98,
		
		"reactivePower": 0.125,
		"activePower": 0.520,
		"powerFactor": 0.98,
		
		"lapseOfTimeTotalEnergy": 10.2,
		"lapseOfTimeImportEnergy": 10.2,
		"lapseOfTimeExportEnergy": 0,
		"lapseOfTimePriceEnergy": 10200.0,
		
		"totalKWh": 16010.2
	}
}
```

### Get limit data
 - Command "getLimitData"
 - Response
```json
{
	"response": "getLimitData",
	"time": 15025622563,
	"data": {
		"maxCurrentLimit": 45,
		"maxVoltageLimit": 250,
		"minVoltageLimit": 190
	},
}
```

### Get purchase data
 - Command "getPurchaseData"
 - Response
```json
{
	"response": "getPurchaseData",
	"time": 15025622563,
	"data": {
		"energyPurchase": 5000,
		"energyPurchaseBalance": 5000,
		"energyPurchaseAlarm": 1000,
		"energyPurchaseStatus": true
	}
}
```

### Get power company data
 - Command "getPowerCompanyData"
 - Response
```json
{
	"response": "getPowerCompanyData",
	"time": 15025622563,
	"data": {
		"startingKWh": 16000,
		"priceKWh": 100
	}
}
```

### Set limit data
 - Command "setLimitsData"
```json
{
	"data": {
		"maxCurrentLimit": 45,
		"maxVoltageLimit": 250,
		"minVoltageLimit": 190
	}
}
```
 - Response
```json
{
	"response": "setLimitsData",
	"time": 15025622563,
	"data": {
		"maxCurrentLimit": 45,
		"maxVoltageLimit": 250,
		"minVoltageLimit": 190
	},
}
```

### Set purchase data
 - Command "setPurchaseData"
```json
{
	"data": {
		"energyPurchase": 5000,
		"energyPurchaseAlarm": 1000,
		"energyPurchaseStatus": true
	}
}
```
 - Response
```json
{
	"response": "setPurchaseData",
	"time": 15025622563,
	"data": {
		"energyPurchase": 5000,
		"energyPurchaseBalance": 5000,
		"energyPurchaseAlarm": 1000,
		"energyPurchaseStatus": true
	}
}
```

### Set power cut data
 - Command "setPowerCutData"
```json
{
	"data": {
		"powerCut": false
	}
}
```
 - Response
```json
{
	"response": "setPowerCutData",
	"data": {
		"powerCut": false,
		"powerCutDetails": "No Power Cut"
	}
}
```

### Set delay to set power cut
 - Command "setDelay"
```json
{
	"data": {
		"delay": 60,
		"delaySetPowerCut": true
	}
}
```
 - Response
```json
{
	"response": "setDelay",
	"data": {
		"delay": 60,
		"delaySetPowerCut": true
	}
}
```

### Set reset
 - Command "setReset"
 - Response
```json
{
	"response": "setReset",
	"time": 15025622563,
	"data": {
		"lapseOfTimeTotalEnergy": 0,
		"lapseOfTimeImportEnergy": 0,
		"lapseOfTimeExportEnergy": 0,
		"lapseOfTimePriceEnergy": 0,
		
		"totalKWh": 16010.2
	}
}
```

### Set power company data
 - Command "setPowerCompanyData"
```json
{
	"data": {
		"startingKWh": 16000,
		"priceKWh": 100
	}
}
```
 - Response
```json
{
	"cmd": "setPowerCompanyData",
	"data": {
		"startingKWh": 16000,
		"priceKWh": 100
	}
}
```

## Raw Message (Only for test)
### Send Message (for test messages).
 - Command "sendRawMessage"
```json
{
	"data": {
		"hex": "48:06:02:01:00:00"
	}
}
```
 - Response
```json
{
	"response": "sendRawMessage",
	"time": 15025622563,
	"data": {
		"hex": "48:15:01:01:01:01:01:00:00:04:01:00:00:10:7E:00:00:00:01:00:F6"
	}
}
```

### Get Raw Message (for messages sent by the smart meter automatically).
```json
{
	"response": "getRawMessage",
	"time": 15025622563,
	"data": {
		"hex": "48:15:01:01:01:01:01:00:00:01:E9:00:00:0B:A0:00:00:00:01:00:F8"
	}
}
```

## JSON Error
### Any error is send by Json Object "error".
```json
"error": {
	"type": "Input data",
	"description": "No bytes received"
}
```
## Note
This documentation is at work.

## License

This code is released under the MIT License.
