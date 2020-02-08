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

#include "JSmartMeter238.h"

//------------------------------------------------------------------------------

#ifdef SM_ENABLE_DEBUG
#ifdef SM_USE_REMOTE_DEBUG
JSmartMeter238::JSmartMeter238(SmartMeter238 &energyMeter, RemoteDebug &debug) : smEnergyMeter(energyMeter), smDebug(debug) {}
#else
JSmartMeter238::JSmartMeter238(SmartMeter238 &energyMeter, HardwareSerial &debug) : smEnergyMeter(energyMeter), smDebug(debug) {}
#endif   // SM_USE_REMOTE_DEBUG
#else   // SM_ENABLE_DEBUG
JSmartMeter238::JSmartMeter238(SmartMeter238 &energyMeter) : smEnergyMeter(energyMeter) {}
#endif   // SM_ENABLE_DEBUG

JSmartMeter238::~JSmartMeter238() {}

void JSmartMeter238::begin(SmartMeter238::smartMeterData &smartMeterData) {
    this->jsonSmartMeterData = &smartMeterData;
}

void JSmartMeter238::setJsonPretty(bool set) {
    this->jsonPretty = set;
}

unsigned int JSmartMeter238::processCmd(jsonCommands cmd, char destination[], const char *jsonData, unsigned int jsonLength) {
    this->errType = JSM_TYPE_NO_ERROR;
    this->errCode = JSM_ERR_NO_ERROR;

	return this->processJSM(cmd, destination, jsonData, jsonLength, false);
}

unsigned int JSmartMeter238::processCmdJson(char destination[], const char *jsonData, unsigned int jsonLength) {
    this->errType = JSM_TYPE_NO_ERROR;
    this->errCode = JSM_ERR_NO_ERROR;

	jsonCommands cmd = this->invalidCmd;

    this->deserializePayload(jsonData, jsonLength);	// Clean buffer doc and load json

	if (this->errType == JSM_TYPE_NO_ERROR) {
		// Check "response" (loop)
		if (this->doc.containsKey("response")) {
			return 0;
		}

		if (!this->doc.containsKey("cmd")) {
			this->errType = JSM_TYPE_PARSE_JSON;
			this->errCode = JSM_ERR_COMMAND_NOT_IN_JSON;
		}
	}

	if (this->errType == JSM_TYPE_NO_ERROR) {
		const char* sCmd = this->doc["cmd"].as<const char*>();

		if (sCmd != NULL) {
			cmd = this->resolveCommand(sCmd);
		}
	}

	return this->processJSM(cmd, destination, "", 0, true);
}

unsigned int JSmartMeter238::processJSM(jsonCommands cmd, char destination[], const char *jsonData, unsigned int jsonLength, bool cmdInJson) {
    if (this->jsonSmartMeterData == nullptr) {
        SM_PRINT_E_LN(F("* Must call begin JSmartMeter238."));

        return 0;
    }

#ifdef SM_ENABLE_RAW_TEST_MSG
    if (cmd != getRawMessage) {
#endif
        SM_PRINT_I_LN(F("In to JSmartMeter238 Library (processCmd)"));
        SM_PRINT_V(F("* Command Process: "));
        SM_PRINT_V_LN(this->commandToText(cmd));
#ifdef SM_ENABLE_RAW_TEST_MSG
    }
#endif

    const char *strErrType = "";
    const char *strErrDescription = "";

    bool jsonError = false;
    bool smError = false;

    switch (cmd) {
        case getPowerCutData: {
            smError = !this->smEnergyMeter.getPowerCutData(this->jsonSmartMeterData, false);

            break;
        }
        case getMeasurementData: {
            smError = !this->smEnergyMeter.getMeasurementData(this->jsonSmartMeterData, false);

            break;
        }
        case getLimitData: {
            smError = !this->smEnergyMeter.getLimitAndPurchaseData(this->jsonSmartMeterData, false);

            break;
        }
        case getPurchaseData: {
            smError = !this->smEnergyMeter.getLimitAndPurchaseData(this->jsonSmartMeterData, false);

            break;
        }
        case getPowerCompanyData: {
            smError = !this->smEnergyMeter.getPowerCompanyData(this->jsonSmartMeterData, false);

            break;
        }

        case setLimitsData: {
            if (!cmdInJson) {
                jsonError = !this->deserializePayload(jsonData, jsonLength);
            }

            if (!jsonError) {
                JsonObject data = this->doc["data"];

                if (data.isNull()) {
                    this->errType = JSM_TYPE_PARSE_JSON;
                    this->errCode = JSM_ERR_DATA_NOT_IN_JSON;

                    jsonError = true;
                } else {
                    if (data.containsKey("maxCurrentLimit") && data.containsKey("maxVoltageLimit") && data.containsKey("minVoltageLimit")) {
                        smError = !this->smEnergyMeter.setLimitsData(data["maxCurrentLimit"].as<float>(), data["maxVoltageLimit"].as<float>(), data["minVoltageLimit"].as<float>(), this->jsonSmartMeterData);
                    } else {
                        this->errType = JSM_TYPE_PARSE_JSON;
                        this->errCode = JSM_ERR_DATA_NOT_VALID;

                        jsonError = true;
                    }
                }
            }

            break;
        }
        case setPurchaseData: {
            if (!cmdInJson) {
                jsonError = !this->deserializePayload(jsonData, jsonLength);
            }

            if (!jsonError) {
                JsonObject data = this->doc["data"];

                if (data.isNull()) {
                    this->errType = JSM_TYPE_PARSE_JSON;
                    this->errCode = JSM_ERR_DATA_NOT_IN_JSON;

                    jsonError = true;
                } else {
                    if (data.containsKey("energyPurchase") && data.containsKey("energyPurchaseAlarm") && data.containsKey("energyPurchaseStatus")) {
                        smError = !this->smEnergyMeter.setPurchaseData(data["energyPurchase"].as<float>(), data["energyPurchaseAlarm"].as<float>(), data["energyPurchaseStatus"].as<bool>(), this->jsonSmartMeterData);
                    } else {
                        this->errType = JSM_TYPE_PARSE_JSON;
                        this->errCode = JSM_ERR_DATA_NOT_VALID;

                        jsonError = true;
                    }
                }
            }

            break;
        }
        case setPowerCutData: {
            if (!cmdInJson) {
                jsonError = !this->deserializePayload(jsonData, jsonLength);
            }

            if (!jsonError) {
                JsonObject data = this->doc["data"];

                if (data.isNull()) {
                    this->errType = JSM_TYPE_PARSE_JSON;
                    this->errCode = JSM_ERR_DATA_NOT_IN_JSON;

                    jsonError = true;
                } else {
                    if (data.containsKey("powerCut")) {
                        smError = !this->smEnergyMeter.setPowerCutData(data["powerCut"].as<bool>(), this->jsonSmartMeterData);
                    } else {
                        this->errType = JSM_TYPE_PARSE_JSON;
                        this->errCode = JSM_ERR_DATA_NOT_VALID;

                        jsonError = true;
                    }
                }
            }

            break;
        }
        case setDelay: {
            if (!cmdInJson) {
                jsonError = !this->deserializePayload(jsonData, jsonLength);
            }

            if (!jsonError) {
                JsonObject data = this->doc["data"];

                if (data.isNull()) {
                    this->errType = JSM_TYPE_PARSE_JSON;
                    this->errCode = JSM_ERR_DATA_NOT_IN_JSON;

                    jsonError = true;
                } else {
                    if (data.containsKey("delaySetPowerCut") && data.containsKey("delay")) {
                        smError = !this->smEnergyMeter.setDelay(data["delaySetPowerCut"].as<bool>(), data["delay"].as<float>(), this->jsonSmartMeterData);
                    } else {
                        this->errType = JSM_TYPE_PARSE_JSON;
                        this->errCode = JSM_ERR_DATA_NOT_VALID;

                        jsonError = true;
                    }
                }
            }

            break;
        }
        case setReset: {
            smError = !this->smEnergyMeter.setReset(this->jsonSmartMeterData);

            break;
        }
        case setPowerCompanyData: {
            if (!cmdInJson) {
                jsonError = !this->deserializePayload(jsonData, jsonLength);
            }

            if (!jsonError) {
                JsonObject data = this->doc["data"];

                if (data.isNull()) {
                    this->errType = JSM_TYPE_PARSE_JSON;
                    this->errCode = JSM_ERR_DATA_NOT_IN_JSON;

                    jsonError = true;
                } else {
                    if (data.containsKey("startingKWh") && data.containsKey("priceKWh")) {
                        smError = !this->smEnergyMeter.setPowerCompanyData(data["startingKWh"].as<float>(), data["priceKWh"].as<float>(), this->jsonSmartMeterData);
                    } else {
                        this->errType = JSM_TYPE_PARSE_JSON;
                        this->errCode = JSM_ERR_DATA_NOT_VALID;

                        jsonError = true;
                    }
                }
            }

            break;
        }
#ifdef SM_ENABLE_RAW_TEST_MSG
        case getRawMessage: {
            smError = !this->smEnergyMeter.processIncomingMessages();

            break;
        }
        case sendRawMessage: {
            if (!cmdInJson) {
                jsonError = !this->deserializePayload(jsonData, jsonLength);
            }

            if (!jsonError) {
                JsonObject data = this->doc["data"];

                if (data.isNull()) {
                    this->errType = JSM_TYPE_PARSE_JSON;
                    this->errCode = JSM_ERR_DATA_NOT_IN_JSON;

                    jsonError = true;
                } else {
                    if (data.containsKey("hex")) {
                        smError = !this->smEnergyMeter.sendHexMessage(data["hex"].as<const char *>());
                    } else {
                        this->errType = JSM_TYPE_PARSE_JSON;
                        this->errCode = JSM_ERR_DATA_NOT_VALID;

                        jsonError = true;
                    }
                }
            }

            break;
        }
#endif
        case invalidCmd: {
            this->errType = JSM_TYPE_PARSE_JSON;
            this->errCode = JSM_ERR_COMMAND_NOT_VALID;

            jsonError = true;

            break;
        }
        default: {
            SM_PRINT_I_LN(F("Out from JSmartMeter238 Library (processCmd)"));

            return 0;
        }
    }

    if (jsonError || smError) {
        if (jsonError) {
            strErrType = this->getTypeStr(true);
            strErrDescription = this->getErrorStr(true);
        } else if (smError) {
            strErrType = this->smEnergyMeter.getTypeStr(true);
            strErrDescription = this->smEnergyMeter.getErrorStr(true);
        }

        SM_PRINT_V(F("* Error:"));
        SM_PRINT_V(F(" type = "));
        SM_PRINT_V(strErrType);
        SM_PRINT_V(F(" / description = "));
        SM_PRINT_V_LN(strErrDescription);
    }

    unsigned int tmpLen = this->serializePayload(cmd, strErrType, strErrDescription, destination);

#ifdef SM_ENABLE_RAW_TEST_MSG
    if (cmd != getRawMessage) {
#endif
        SM_PRINT_I_LN(F("Out from JSmartMeter238 Library (processCmd)"));
#ifdef SM_ENABLE_RAW_TEST_MSG
    }
#endif

    return tmpLen;
}

unsigned int JSmartMeter238::serializePayload(jsonCommands cmd, const char *strErrType, const char *strErrDescription, char destination[]) {
    this->doc.clear();

    this->doc["response"] = this->commandToText(cmd);
    this->doc["time"] = millis();

    JsonObject data;

    switch (cmd) {
        case getPowerCutData: {
            data = this->doc.createNestedObject("data");

            data["powerCut"] = this->jsonSmartMeterData->powerCutData.data.powerCut;
            data["powerCutDetails"] = this->jsonSmartMeterData->powerCutData.data.powerCutDetails;

            data["delay"] = this->jsonSmartMeterData->powerCutData.data.delay;
            data["delaySetPowerCut"] = this->jsonSmartMeterData->powerCutData.data.delaySetPowerCut;

            break;
        }
        case getMeasurementData: {
            data = this->doc.createNestedObject("data");

            data["current"] = this->round(this->jsonSmartMeterData->measurementData.data.current, 3);
            data["voltage"] = this->round(this->jsonSmartMeterData->measurementData.data.voltage, 1);
            data["frequency"] = this->round(this->jsonSmartMeterData->measurementData.data.frequency, 2);

            data["reactivePower"] = this->round(this->jsonSmartMeterData->measurementData.data.reactivePower, 3);
            data["activePower"] = this->round(this->jsonSmartMeterData->measurementData.data.activePower, 3);
            data["powerFactor"] = this->round(this->jsonSmartMeterData->measurementData.data.powerFactor, 2);

            data["lapseOfTimeTotalEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimeTotalEnergy, 2);
            data["lapseOfTimeImportEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimeImportEnergy, 2);
            data["lapseOfTimeExportEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimeExportEnergy, 2);
            data["lapseOfTimePriceEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimePriceEnergy, 1);

            data["totalKWh"] = this->round(this->jsonSmartMeterData->measurementData.data.totalKWh, 2);

            break;
        }

        case getLimitData:
        case setLimitsData: {
            data = this->doc.createNestedObject("data");

            data["maxCurrentLimit"] = this->jsonSmartMeterData->limitAndPurchaseData.data.maxCurrentLimit;
            data["maxVoltageLimit"] = this->jsonSmartMeterData->limitAndPurchaseData.data.maxVoltageLimit;
            data["minVoltageLimit"] = this->jsonSmartMeterData->limitAndPurchaseData.data.minVoltageLimit;

            break;
        }
        case getPurchaseData:
        case setPurchaseData: {
            data = this->doc.createNestedObject("data");

            data["energyPurchase"] = this->round(this->jsonSmartMeterData->limitAndPurchaseData.data.energyPurchase, 2);
            data["energyPurchaseBalance"] = this->round(this->jsonSmartMeterData->limitAndPurchaseData.data.energyPurchaseBalance, 2);
            data["energyPurchaseAlarm"] = this->round(this->jsonSmartMeterData->limitAndPurchaseData.data.energyPurchaseAlarm, 2);
            data["energyPurchaseStatus"] = this->jsonSmartMeterData->limitAndPurchaseData.data.energyPurchaseStatus;

            break;
        }
        case setPowerCutData: {
            data = this->doc.createNestedObject("data");

            data["powerCut"] = this->jsonSmartMeterData->powerCutData.data.powerCut;
            data["powerCutDetails"] = this->jsonSmartMeterData->powerCutData.data.powerCutDetails;

            break;
        }
        case setDelay: {
            data = this->doc.createNestedObject("data");

            data["delay"] = this->jsonSmartMeterData->powerCutData.data.delay;
            data["delaySetPowerCut"] = this->jsonSmartMeterData->powerCutData.data.delaySetPowerCut;

            break;
        }
        case setReset: {
            data = this->doc.createNestedObject("data");

            data["lapseOfTimeTotalEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimeTotalEnergy, 2);
            data["lapseOfTimeImportEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimeImportEnergy, 2);
            data["lapseOfTimeExportEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimeExportEnergy, 2);
            data["lapseOfTimePriceEnergy"] = this->round(this->jsonSmartMeterData->measurementData.data.lapseOfTimePriceEnergy, 2);

            data["totalKWh"] = this->round(this->jsonSmartMeterData->measurementData.data.totalKWh, 2);

            break;
        }
        case getPowerCompanyData:
        case setPowerCompanyData: {
            data = this->doc.createNestedObject("data");

            data["startingKWh"] = this->round(this->jsonSmartMeterData->powerCompanyData.data.startingKWh, 2);
            data["priceKWh"] = this->round(this->jsonSmartMeterData->powerCompanyData.data.priceKWh, 2);

            break;
        }
#ifdef SM_ENABLE_RAW_TEST_MSG
        case getRawMessage: {
            const char *tmpStr = this->smEnergyMeter.getIncomingHexMessage();

            if (strlen(tmpStr) > 0) {
                data = this->doc.createNestedObject("data");

                data["hex"] = tmpStr;
            } else {
                return 0;   // It is necessary to return 0, if there are no messages. This function is constantly called.
            }

            break;
        }
        case sendRawMessage: {
            data = this->doc.createNestedObject("data");

            data["hex"] = this->smEnergyMeter.getIncomingHexMessage();   // Get response for command send, save in getHexMessage()

            break;
        }
#endif
        case invalidCmd: {
            break;
        }
        default: {
            return 0;
        }
    }

    JsonObject error;

    if (strlen(strErrType) > 0 && strlen(strErrDescription) > 0) {
        error = this->doc.createNestedObject("error");

        error["type"] = strErrType;
        error["description"] = strErrDescription;
    }

    if (this->jsonPretty) {
        return serializeJsonPretty(this->doc, destination, JSM_JSON_BUFFER);
    }

    return serializeJson(this->doc, destination, JSM_JSON_BUFFER);
}

bool JSmartMeter238::deserializePayload(const char *jsonData, unsigned int jsonLength) {
    // Clean buffer doc and load json
    DeserializationError error = deserializeJson(this->doc, jsonData, jsonLength);

    // Check valid Json
    if (error) {
        this->errType = JSM_TYPE_PARSE_JSON;

        switch (error.code()) {
            case DeserializationError::TooDeep: {
                this->errCode = JSM_ERR_TOO_DEEP;

                break;
            }
            case DeserializationError::NoMemory: {
                this->errCode = JSM_ERR_NO_MEMORY;

                break;
            }
            case DeserializationError::InvalidInput: {
                this->errCode = JSM_ERR_INVALID_INPUT;

                break;
            }
            case DeserializationError::IncompleteInput: {
                this->errCode = JSM_ERR_INCOMPLETE_INPUT;

                break;
            }
            case DeserializationError::NotSupported: {
                this->errCode = JSM_ERR_NOT_SUPPORTED;

                break;
            }
            default: {
                this->errCode = JSM_ERR_OTHER_ARDUINOJSON;
            }
        }
    }

    return !error;
}

JSmartMeter238::jsonCommands JSmartMeter238::resolveCommand(const char *cmd) {
    if (strcmp(cmd, this->strGetPowerCutData) == 0)
        return getPowerCutData;
    else if (strcmp(cmd, this->strGetMeasurementData) == 0)
        return getMeasurementData;
    else if (strcmp(cmd, this->strGetLimitData) == 0)
        return getLimitData;
    else if (strcmp(cmd, this->strGetPurchaseData) == 0)
        return getPurchaseData;
    else if (strcmp(cmd, this->strGetPowerCompanyData) == 0)
        return getPowerCompanyData;
    else if (strcmp(cmd, this->strSetLimitsData) == 0)
        return setLimitsData;
    else if (strcmp(cmd, this->strSetPurchaseData) == 0)
        return setPurchaseData;
    else if (strcmp(cmd, this->strSetPowerCutData) == 0)
        return setPowerCutData;
    else if (strcmp(cmd, this->strSetDelay) == 0)
        return setDelay;
    else if (strcmp(cmd, this->strSetReset) == 0)
        return setReset;
    else if (strcmp(cmd, this->strSetPowerCompanyData) == 0)
        return setPowerCompanyData;
#ifdef SM_ENABLE_RAW_TEST_MSG
    else if (strcmp(cmd, this->strGetRawMessage) == 0)
        return getRawMessage;
    else if (strcmp(cmd, this->strSendRawMessage) == 0)
        return sendRawMessage;
#endif

    return invalidCmd;
}

const char *JSmartMeter238::commandToText(jsonCommands cmd) {
    switch (cmd) {
        case getPowerCutData:
            return this->strGetPowerCutData;
        case getMeasurementData:
            return this->strGetMeasurementData;
        case getLimitData:
            return this->strGetLimitData;
        case getPurchaseData:
            return this->strGetPurchaseData;
        case getPowerCompanyData:
            return this->strGetPowerCompanyData;
        case setLimitsData:
            return this->strSetLimitsData;
        case setPurchaseData:
            return this->strSetPurchaseData;
        case setPowerCutData:
            return this->strSetPowerCutData;
        case setDelay:
            return this->strSetDelay;
        case setReset:
            return this->strSetReset;
        case setPowerCompanyData:
            return this->strSetPowerCompanyData;
#ifdef SM_ENABLE_RAW_TEST_MSG
        case getRawMessage:
            return this->strGetRawMessage;
        case sendRawMessage:
            return this->strSendRawMessage;
#endif
        case invalidCmd:
            return this->strInvalid;
    }

    return "";
}

JSmartMeter238::jsmErrorType JSmartMeter238::getErrType(bool clear) {
    jsmErrorType tmp = this->errType;

    if (clear) {
        this->clearErrType();
    }

    return tmp;
}

JSmartMeter238::jsmErrorCode JSmartMeter238::getErrCode(bool clear) {
    jsmErrorCode tmp = this->errCode;

    if (clear) {
        this->clearErrCode();
    }

    return tmp;
}

void JSmartMeter238::clearErrType() {
    this->errType = JSM_TYPE_NO_ERROR;
}

void JSmartMeter238::clearErrCode() {
    this->errCode = JSM_ERR_NO_ERROR;
}

char *JSmartMeter238::getTypeStr(bool clear) {
    const char *textTable = jsmStrTypeTable[this->getErrType(clear)];

    uint8_t sizeString = strlen_P(textTable);

    for (uint8_t i = 0; i < sizeString; i++) {
        prtStrType[i] = pgm_read_byte_near(textTable + i);
    }

    prtStrType[sizeString] = 0;   // '\0'

    return prtStrType;
}

char *JSmartMeter238::getErrorStr(bool clear) {
    const char *textTable = jsmStrErrTable[this->getErrCode(clear)];

    uint8_t sizeString = strlen_P(textTable);

    for (uint8_t i = 0; i < sizeString; i++) {
        prtStrError[i] = pgm_read_byte_near(textTable + i);
    }

    prtStrError[sizeString] = 0;   // '\0'

    return prtStrError;
}

char *JSmartMeter238::round(float value, uint8_t decimalPlaces) {
    static char buf[16];

    float factor = pow(10, decimalPlaces);

    float val = ((int)(value * factor + 0.5)) / factor;

    return dtostrf(val, (decimalPlaces + 2), decimalPlaces, buf);
}
