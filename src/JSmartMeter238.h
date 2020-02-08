/*
This library provides a convenient way (JSON) to interact with the SmartMeter238 library.
Reading via Hardware Serial
2020 (development with PlatformIO IDE for VSCode & esp8266 core)

MIT License

Copyright (c) 2020 Rodrigo González

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

//------------------------------------------------------------------------------
#ifndef JSmartMeter238_h
#define JSmartMeter238_h
//------------------------------------------------------------------------------

#define JSM_VERSION "1.0.0-beta1"

#include <Arduino.h>
#include <HardwareSerial.h>

#include <ArduinoJson.h>     // For sending JSON messages https://bblanchon.github.io/ArduinoJson/
#include <RemoteDebug.h>     // https://github.com/JoaoLopesF/RemoteDebug
#include <SmartMeter238.h>   // For reading DDS238-4 W Wifi Smart meter (SM)

//------------------------------------------------------------------------------
// DEFAULTS
//------------------------------------------------------------------------------

#ifndef JSM_JSON_BUFFER
#define JSM_JSON_BUFFER 512
#endif   // JSM_JSON_BUFFER

//------------------------------------------------------------------------------

// Type error text
const char jsmStrTypeNoError[] PROGMEM = {"No error type"};
const char jsmStrTypeParse[] PROGMEM = {"Parse Json"};

const char *const jsmStrTypeTable[] PROGMEM = {
	jsmStrTypeNoError,
	jsmStrTypeParse
};

// Error text
const char jsmStrErrNoError[] PROGMEM = {"No Errors"};

const char jsmStrErrTooDeep[] PROGMEM = {"The nesting limit was reached"};
const char jsmStrErrNoMemory[] PROGMEM = {"Small JsonDocument, increase its capacity"};
const char jsmStrErrInvalidInput[] PROGMEM = {"The input is not recognized"};
const char jsmStrErrIncompleteInput[] PROGMEM = {"The end of the input is missing"};
const char jsmStrErrNotSupported[] PROGMEM = {"Features in Json not supported by the parser"};
const char jsmStrErrOther[] PROGMEM = {"No info about this error"};

const char jsmStrErrDataNotInJson[] PROGMEM = {"Data not present"};
const char jsmStrErrDataNotValid[] PROGMEM = {"Data not valid"};
const char jsmStrErrCommandNotInJson[] PROGMEM = {"Command is not present"};
const char jsmStrErrCommandNotValid[] PROGMEM = {"Command not valid"};

const char *const jsmStrErrTable[] PROGMEM = {
	jsmStrErrNoError,
	jsmStrErrTooDeep,
	jsmStrErrNoMemory,
	jsmStrErrInvalidInput,
	jsmStrErrIncompleteInput,
	jsmStrErrNotSupported,
	jsmStrErrOther,
	jsmStrErrDataNotInJson,
	jsmStrErrDataNotValid,
    jsmStrErrCommandNotInJson,	
    jsmStrErrCommandNotValid
};

class JSmartMeter238 {
   public:
    enum jsonCommands {
        getPowerCutData,
        getMeasurementData,
        getLimitData,
        getPurchaseData,
        getPowerCompanyData,

        setLimitsData,
        setPurchaseData,
        setPowerCutData,
        setDelay,
        setReset,
        setPowerCompanyData,
#ifdef SM_ENABLE_RAW_TEST_MSG
        getRawMessage,
        sendRawMessage,
#endif
        invalidCmd
    };

    enum jsmErrorType {
        JSM_TYPE_NO_ERROR,
        JSM_TYPE_PARSE_JSON
    };

    enum jsmErrorCode {
        JSM_ERR_NO_ERROR,             // no error
        JSM_ERR_TOO_DEEP,             // The nesting limit was reached
        JSM_ERR_NO_MEMORY,            // The JsonDocument is too small; you need to increase its capacity
        JSM_ERR_INVALID_INPUT,        // The input is not recognized
        JSM_ERR_INCOMPLETE_INPUT,     // The end of the input is missing
        JSM_ERR_NOT_SUPPORTED,        // The document included features not supported by the parser
        JSM_ERR_OTHER_ARDUINOJSON,    // No info about this error
        JSM_ERR_DATA_NOT_IN_JSON,     // Data not present
        JSM_ERR_DATA_NOT_VALID,       // Data not valid
        JSM_ERR_COMMAND_NOT_IN_JSON,  // Command not present	
        JSM_ERR_COMMAND_NOT_VALID     // Commmand not valid
    };

#ifdef SM_ENABLE_DEBUG
#ifdef SM_USE_REMOTE_DEBUG
    JSmartMeter238(SmartMeter238 &energyMeter, RemoteDebug &debug);
#else
    JSmartMeter238(SmartMeter238 &energyMeter, HardwareSerial &debug);
#endif   // SM_USE_REMOTE_DEBUG
#else
    JSmartMeter238(SmartMeter238 &energyMeter);
#endif   // SM_ENABLE_DEBUG

    virtual ~JSmartMeter238();

    void setJsonPretty(bool set);

    void begin(SmartMeter238::smartMeterData &smartMeterData);

    unsigned int processCmd(jsonCommands cmd, char destination[], const char *jsonData, unsigned int jsonLength);
    
    unsigned int processCmdJson(char destination[], const char *jsonData, unsigned int jsonLength);

    jsonCommands resolveCommand(const char *cmd);

   private:
    bool jsonPretty = false;

    SmartMeter238::smartMeterData *jsonSmartMeterData;

    jsmErrorType errType = JSM_TYPE_NO_ERROR;
    jsmErrorCode errCode = JSM_ERR_NO_ERROR;

    char *round(float value, uint8_t decimalPlaces);

    char prtStrType[SM_MAX_STR_LENGTH_TYPE];
    char prtStrError[SM_MAX_STR_LENGTH_ERROR];

    const char *strGetPowerCutData = "getPowerCutData";
    const char *strGetMeasurementData = "getMeasurementData";
    const char *strGetLimitData = "getLimitData";
    const char *strGetPurchaseData = "getPurchaseData";
    const char *strGetPowerCompanyData = "getPowerCompanyData";

    const char *strSetLimitsData = "setLimitsData";
    const char *strSetPurchaseData = "setPurchaseData";
    const char *strSetPowerCutData = "setPowerCutData";
    const char *strSetDelay = "setDelay";
    const char *strSetReset = "setReset";
    const char *strSetPowerCompanyData = "setPowerCompanyData";
#ifdef SM_ENABLE_RAW_TEST_MSG
    const char *strGetRawMessage = "getRawMessage";
    const char *strSendRawMessage = "sendRawMessage";
#endif
    const char *strInvalid = "commandInvalid";

    bool deserializePayload(const char *jsonData, unsigned int jsonLength);

    unsigned int serializePayload(jsonCommands cmd, const char *strErrType, const char *strErrDescription, char destination[]);

    const char *commandToText(jsonCommands cmd);

    unsigned int processJSM(jsonCommands cmd, char destination[], const char *jsonData, unsigned int jsonLength, bool cmdInJson);

    jsmErrorType getErrType(bool clear = false);
    jsmErrorCode getErrCode(bool clear = false);

    void clearErrType();
    void clearErrCode();

    char *getTypeStr(bool clear = false);
    char *getErrorStr(bool clear = false);

    SmartMeter238 &smEnergyMeter;

#ifdef SM_ENABLE_DEBUG
#ifdef SM_USE_REMOTE_DEBUG
    RemoteDebug &smDebug;
#else
    HardwareSerial &smDebug;
#endif   // SM_USE_REMOTE_DEBUG
#endif   // SM_ENABLE_DEBUG

    StaticJsonDocument<JSM_JSON_BUFFER> doc;
};
#endif   // JSmartMeter238_h
