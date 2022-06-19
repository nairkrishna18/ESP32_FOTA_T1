#if 1  // OTA.cpp Starts here.................
#ifndef _OTA_H_
    #define _OTA_H_
    #include "OTA.h"
#endif

char* test_root_ca= "-----BEGIN CERTIFICATE-----\n"
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
"-----END CERTIFICATE-----\n";

const char* ssid     = "TROJAN_TP";     // your network SSID (name of wifi network)
const char* password = "9833110550"; // your network password
// const char* ssid     = "CG-PLENT_0002";     // your network SSID (name of wifi network)
// const char* password = "1234567890"; // your network password

bool bStartOta = 0;
bool shouldExecuteFirmwareUpdate = 0;
uint8_t ui08OtaStates = 0;
uint16_t ui16TempCounter = 0;
uint16_t retries = 0;
uint16_t ui16ReootingCounter = 0;
extern bool bErrorInUpdating;
extern bool bUpdateComplete;

WiFiClientSecure clientForOta;
secureEsp32FOTA secureEsp32FOTA("esp32-fota-https", VERSION_NO);



void TaskOTAcode( void * parameter )
{
    while(1)
    {
        FnOtaStates();
    }
}

void FnOtaStates(void)
{
    switch(ui08OtaStates)
    {
        case INIT_WIFI :
        {
            Serial.print("Connecting to ");
            Serial.println(ssid);
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid, password);
            retries = 0;
            ui08OtaStates = WAIT_FOR_CONNECT;
        }
        break;

        case WAIT_FOR_CONNECT :
        {
            if(WiFi.status() != WL_CONNECTED)
            {
                if(retries >= 50)
                {
                    retries = 0;
                    Serial.print(".\n");
                    Serial.print("\n");
                    //ui08OtaStates = WAIT_FOR_CONNECT;
                }
                else
                {
                    delay(100);
                    retries++;
                    Serial.print(".");
                }
            }
            else
            {
                Serial.println("");
                Serial.print("Connected, IP: ");
                Serial.println(WiFi.localIP());
                Serial.println("Waiting for Update.....\n");
                ui08OtaStates = CHECK_FOR_UPDATE_MESSAGE;
            }
        }
        break;

        case CHECK_FOR_UPDATE_MESSAGE :
        {
            if(WiFi.status() != WL_CONNECTED)
            {
                Serial.println("WiFi Disconnected......!\n");
                Serial.println("Reconnecting...\n");
                WiFi.begin(ssid, password);
                ui08OtaStates = WAIT_FOR_CONNECT;
            }
            else
            {
                delay(1000);
                Serial.println(".\n");
            }
            bStartOta = 1;
            ui08OtaStates = INIT_FOR_UPDATE;
        }
        break;

        case INIT_FOR_UPDATE :
        {
             if(WiFi.status() != WL_CONNECTED)
            {
                Serial.println("WiFi Disconnected......!\n");
                Serial.println("Reconnecting...\n");
                WiFi.begin(ssid, password);
                ui08OtaStates = WAIT_FOR_CONNECT;
            }
            else
            {
                secureEsp32FOTA._host="raw.githubusercontent.com";
                
                // secureEsp32FOTA._descriptionOfFirmwareURL="/nairkrishna18/PLENT_MASTER/main/CG_PLENT_MASTER_FW_DETAILS.json";
                secureEsp32FOTA._descriptionOfFirmwareURL=JSON_ADDRESS;
                secureEsp32FOTA._certificate=test_root_ca;
                secureEsp32FOTA.clientForOta=clientForOta;
                ui16TempCounter = 0;
                ui08OtaStates = CHECK_VERSION;
            }            
        }
        break;

        case CHECK_VERSION :
        {
             if(WiFi.status() != WL_CONNECTED)
            {
                Serial.println("WiFi Disconnected......!\n");
                Serial.println("Reconnecting...\n");
                WiFi.begin(ssid, password);
                ui16TempCounter = 101;
                ui08OtaStates = WAIT_FOR_CONNECT;
            }
            else
            {
                if(ui16TempCounter >= 100)
                {
                    shouldExecuteFirmwareUpdate=secureEsp32FOTA.execHTTPSCheck();
                    // shouldExecuteFirmwareUpdate=secureEsp32FOTA.execHTTPSCheckSpecial();
                    if(shouldExecuteFirmwareUpdate)
                    {
                        ui08OtaStates = START_OTA;
                    }
                    else
                    {
                        Serial.println("Running on latest version...\n");
                    }
                    ui16TempCounter = 0;
                }
                else
                {
                    ui16TempCounter++;
                    delay(100);
                }
            }            
        }
        break;

        case START_OTA :
        {
            if(WiFi.status() != WL_CONNECTED)
            {
                Serial.println("WiFi Disconnected......!\n");
                Serial.println("Reconnecting...\n");
                WiFi.begin(ssid, password);
                ui08OtaStates = WAIT_FOR_CONNECT;
            }
            else
            {
                if(shouldExecuteFirmwareUpdate)
                {
                    Serial.println("Firmware update available!");
                    secureEsp32FOTA.executeOTA();
                    if(bUpdateComplete == 1)
                    {
                        ui16TempCounter = 0;
                        ui16ReootingCounter = 5;
                        bUpdateComplete = 0;
                        ui08OtaStates = REBOOT_SYSTEM;
                    }
                    else if(bErrorInUpdating == 1)
                    {
                        bErrorInUpdating = 0;
                        ui08OtaStates = CHECK_FOR_UPDATE_MESSAGE;
                    }
                }
            }   
        }
        break;

        case REBOOT_SYSTEM :
        {
            if(ui16TempCounter >= 100)
            {
                ui16ReootingCounter--;
                Serial.println("Rebooting in  : " + String(ui16ReootingCounter) + " Sec");
                ui16TempCounter = 0;
            }
            else
            {
                delay(10);
                ui16TempCounter++;
            }
            if(ui16ReootingCounter <= 0)
            {
                Serial.println("!..........REBOOT..........!");
                ESP.restart();
            }
        }
        break;

    }
}
#endif // OTA.cpp Ends here.................

#if 1  // esp32fota_edited.cpp Starts here............
/*
   esp32 firmware OTA
   Date: December 2018
   Author: Chris Joyce <https://github.com/chrisjoyce911/esp32FOTA/esp32FOTA>
   Purpose: Perform an OTA update from a bin located on a webserver (HTTP Only)
   Edited By: AKSHAY ATKAR
*/




// #include "Update_Edited.h"

#include "Arduino.h"
#include <WiFi.h>
#include <HTTPClient.h>

#include "ArduinoJson.h"

#include <WiFiClientSecure.h>

#include <esp_task_wdt.h>           //ARA191121

bool bErrorInUpdating = 0;          //ARA191121
bool bUpdateComplete = 0;           //ARA191121

esp32FOTA::esp32FOTA(String firwmareType, float firwmareVersion)
{
    _firwmareType = firwmareType;
    _firwmareVersion = firwmareVersion;
    useDeviceID = false;
}
//-----------------------ARA271121-----------------------------------------
// esp32FOTA::esp32FOTASpecial(String firwmareType_, float_t firwmareVersion_)
// {
//     _firwmareType = firwmareType_;
//     _firwmareVersion = firwmareVersion_;
//     useDeviceID = false;
// }
//----------------------ARA271121------------------------------------------


static void splitHeader(String src, String &header, String &headerValue)
{
    int idx = 0;

    idx = src.indexOf(':');
    header = src.substring(0, idx);
    headerValue = src.substring(idx + 1, src.length());
    headerValue.trim();

    return;
}



/*--------ARA261121----------------------------------------------------------------------*/
// OTA Logic
void esp32FOTA::execOTASpecial()
{

    WiFiClient client;
    int contentLength = 0;
    int contentLengthForPerc = 0;
    bool isValidContentType = false;
    bool gotHTTPStatus = false;

    Serial.println("Connecting to: " + String(_host));
    // Connect to Webserver
    if (client.connect(_host.c_str(), _port))
    {
        // Connection Succeed.
        // Fetching the bin
        Serial.println("Fetching Bin: " + String(_bin));

        // Get the contents of the bin file
        client.print(String("GET ") + _bin + " HTTP/1.1\r\n" +
                     "Host: " + _host + "\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 5000)
            {
                bUpdateComplete = 1;                   //ARA261121
                Serial.println("Client Timeout !");
                client.stop();
                return;
            }
        }

        while (client.available())
        {
            String header, headerValue;
            // read line till /n
            String line = client.readStringUntil('\n');
            Serial.println(line);           //ARA031221
            // remove space, to check if the line is end of headers
            line.trim();

            if (!line.length())
            {
                //headers ended
                break; // and get the OTA started
            }

            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1"))
            {
                if (line.indexOf("200") < 0)
                {
                    bUpdateComplete = 1;                   //ARA261121
                    Serial.println("Firmware.bin file not present in server/Winscp/Git"); //KRN101221
                    Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                    client.stop();
                    break;
                }
                gotHTTPStatus = true;
            }

            if (false == gotHTTPStatus)
            {
                continue;
            }

            splitHeader(line, header, headerValue);

            // extract headers here
            // Start with content length
            if (header.equalsIgnoreCase("Content-Length"))
            {
                contentLength = headerValue.toInt();
                contentLengthForPerc = contentLength;           //ARA031221
                Serial.println("Got " + String(contentLength) + " bytes from server");
                continue;
            }

            // Next, the content type
            if (header.equalsIgnoreCase("Content-type"))
            {
                String contentType = headerValue;
                Serial.println("Got " + contentType + " payload.");
                if (contentType == "application/octet-stream")
                {
                    isValidContentType = true;
                }
            }
        }
    }
    else
    {
        // Connect to webserver failed
        // May be try?
        // Probably a choppy network?
        bUpdateComplete = 1;                   //ARA261121
        Serial.println("Connection to " + String(_host) + " failed. Please check your setup");
        // retry??
        // execOTA();
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);

        // If yes, begin
        if (canBegin)
        {
            Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            // No activity would appear on the Serial monitor
            // So be patient. This may take 2 - 5mins to complete
            size_t written = Update.writeStreamEdited(client,contentLengthForPerc);
                // if(Update.remaining())
                // {
                //     written = Update.writeStreamEdited2(client,contentLengthForPerc);
                // }
            if (written == contentLength)
            {
                Serial.println("Written : " + String(written) + " successfully");
            }
            else
            {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                // retry??
                // execOTA();
            }

            if (Update.end())
            {
                Serial.println("OTA done!");
                if (Update.isFinished())
                {
                    Serial.println("Update successfully completed. Rebooting.");
                    bUpdateComplete = 1;                   //ARA261121
                    //ESP.restart();
                }
                else
                {
                    bErrorInUpdating = 1;                   //ARA261121
                    Serial.println("Update not finished? Something went wrong!");
                }
            }
            else
            {
                bErrorInUpdating = 1;                   //ARA261121
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            }
        }
        else
        {
            // not enough space to begin OTA
            // Understand the partitions and
            // space availability
            bErrorInUpdating = 1;                   //ARA261121
            Serial.println("Not enough space to begin OTA");
            client.flush();
        }
    }
    else
    {
        Serial.println("There was no content in the response");
        bErrorInUpdating = 1;                   //ARA261121
        client.flush();
    }
}
/*--------ARA261121----------------------------------------------------------------------*/



// OTA Logic
void esp32FOTA::execOTA()
{

    WiFiClient client;
    int contentLength = 0;
    bool isValidContentType = false;
    bool gotHTTPStatus = false;

    Serial.println("Connecting to: " + String(_host));
    // Connect to Webserver
    if (client.connect(_host.c_str(), _port))
    {
        // Connection Succeed.
        // Fetching the bin
        Serial.println("Fetching Bin: " + String(_bin));

        // Get the contents of the bin file
        client.print(String("GET ") + _bin + " HTTP/1.1\r\n" +
                     "Host: " + _host + "\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 5000)
            {
                Serial.println("Client Timeout !");
                client.stop();
                return;
            }
        }

        while (client.available())
        {
            String header, headerValue;
            // read line till /n
            String line = client.readStringUntil('\n');
            // remove space, to check if the line is end of headers
            line.trim();

            if (!line.length())
            {
                //headers ended
                break; // and get the OTA started
            }

            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1"))
            {
                if (line.indexOf("200") < 0)
                {
                    Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                    client.stop();
                    break;
                }
                gotHTTPStatus = true;
            }

            if (false == gotHTTPStatus)
            {
                continue;
            }

            splitHeader(line, header, headerValue);

            // extract headers here
            // Start with content length
            if (header.equalsIgnoreCase("Content-Length"))
            {
                contentLength = headerValue.toInt();
                Serial.println("Got " + String(contentLength) + " bytes from server");
                continue;
            }

            // Next, the content type
            if (header.equalsIgnoreCase("Content-type"))
            {
                String contentType = headerValue;
                Serial.println("Got " + contentType + " payload.");
                if (contentType == "application/octet-stream")
                {
                    isValidContentType = true;
                }
            }
        }
    }
    else
    {
        // Connect to webserver failed
        // May be try?
        // Probably a choppy network?
        Serial.println("Connection to " + String(_host) + " failed. Please check your setup");
        // retry??
        // execOTA();
    }

    // Check what is the contentLength and if content type is `application/octet-stream`
    Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

    // check contentLength and content type
    if (contentLength && isValidContentType)
    {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);

        // If yes, begin
        if (canBegin)
        {
            Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
            // No activity would appear on the Serial monitor
            // So be patient. This may take 2 - 5mins to complete
            size_t written = Update.writeStream(client);

            if (written == contentLength)
            {
                Serial.println("Written : " + String(written) + " successfully");
            }
            else
            {
                Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                // retry??
                // execOTA();
            }

            if (Update.end())
            {
                Serial.println("OTA done!");
                if (Update.isFinished())
                {
                    Serial.println("Update successfully completed. Rebooting.");
                    ESP.restart();
                }
                else
                {
                    Serial.println("Update not finished? Something went wrong!");
                }
            }
            else
            {
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            }
        }
        else
        {
            // not enough space to begin OTA
            // Understand the partitions and
            // space availability
            Serial.println("Not enough space to begin OTA");
            client.flush();
        }
    }
    else
    {
        Serial.println("There was no content in the response");
        client.flush();
    }
}


/*--------ARA261121----------------------------------------------------------------------*/
bool esp32FOTA::execHTTPcheckSpecial()
{

    String useURL;
    static uint16_t ivar1, ivar2;

    if (useDeviceID)
    {
        // String deviceID = getDeviceID() ;
        useURL = checkURL + "?id=" + getDeviceID();
    }
    else
    {
        useURL = checkURL;
    }

    _port = 80;

    Serial.println("Getting HTTP");
    Serial.println(useURL);
    Serial.println("------");
    if ((WiFi.status() == WL_CONNECTED))
    { //Check the current connection status

        HTTPClient http;

        http.begin(useURL);        //Specify the URL
        int httpCode = http.GET(); //Make the request
		Serial.println("httpCodeRecevied");
		Serial.print(httpCode,DEC);
        if (httpCode == 200)
        { //Check is a file was returned

            String payload = http.getString();

            int str_len = payload.length() + 1;
            char JSONMessage[str_len];
            payload.toCharArray(JSONMessage, str_len);
			Serial.print(JSONMessage);
            StaticJsonDocument<500> JSONDocument; //Memory pool
            DeserializationError err = deserializeJson(JSONDocument, JSONMessage);

            if (err)
            { //Check for errors in parsing
                Serial.println("Parsing failed");
                delay(5000);
                return false;
            }

            const char *pltype = JSONDocument["type"];
            float plversion = JSONDocument["version"].as<float>();
            const char *plhost = JSONDocument["host"];
            _port = JSONDocument["port"];
            const char *plbin = JSONDocument["bin"];

            
            SturOtaJsonReceivHttp.type = JSONDocument["type"].as<String>();
            SturOtaJsonReceivHttp.Date = JSONDocument["type"].as<String>();
            SturOtaJsonReceivHttp.For = JSONDocument["For"].as<String>();
            SturOtaJsonReceivHttp.host = JSONDocument["host"].as<String>();
            SturOtaJsonReceivHttp.version = JSONDocument["version"].as<float>();
            SturOtaJsonReceivHttp.bin = JSONDocument["bin"].as<String>();

            String jshost(plhost);
            String jsbin(plbin);

            _host = jshost;
            _bin = jsbin;

            ivar1 = plversion * 100;
            ivar2 = _firwmareVersion * 100;

            String fwtype(pltype);
            Serial.print("plversion=");
            Serial.println(plversion);
            Serial.print("_firwmareVersion=");
            Serial.println(_firwmareVersion);

            Serial.print("ivar1=");
            Serial.println(ivar1);
            Serial.print("ivar2=");
            Serial.println(ivar2);

            #if 1
                if ((ivar1  == ivar2)||(fwtype != _firwmareType))
                // if ((ivar1  == ivar2))
                // if ((_firwmareVersion  == plversion))
                {
                    Serial.println("FAILED TO COMPARE.........!!!");
                    return false;                
                }
                else
                if ((ivar1  != ivar2)&&(fwtype == _firwmareType))
                {
                    Serial.println("NEED TO UPDATE.........!!!");
                    return true;
                }
           

            // if ((fwtype == _firwmareType))
            // {            
            //     if ((_firwmareVersion  != plversion))
            //     {
            //         Serial.println("NEED TO UPDATE.........!!!");
            //         return true;
            //     }
            //     else
            //     {
            //         Serial.println("FAILED TO COMPARE.........!!!");
            //         return false;
            //     }
            // }
            #else
                if ((fwtype == _firwmareType))
                if ((_firwmareVersion  != plversion))
                {
                    Serial.println("NEED TO UPDATE.........!!!");
                    return true;
                }
                else
                {
                    Serial.println("FAILED TO COMPARE.........!!!");
                    return false;
                }
            #endif
        }

        else
        {
            Serial.println("Error on HTTP request");
            return false;
        }

        http.end(); //Free the resources
        return false;
    }
    return false;
}
/*--------ARA261121----------------------------------------------------------------------*/




bool esp32FOTA::execHTTPcheck()
{

    String useURL;

    if (useDeviceID)
    {
        // String deviceID = getDeviceID() ;
        useURL = checkURL + "?id=" + getDeviceID();
    }
    else
    {
        useURL = checkURL;
    }

    _port = 80;

    Serial.println("Getting HTTP");
    Serial.println(useURL);
    Serial.println("------");
    if ((WiFi.status() == WL_CONNECTED))
    { //Check the current connection status

        HTTPClient http;

        http.begin(useURL);        //Specify the URL
        int httpCode = http.GET(); //Make the request
		Serial.println("httpCodeRecevied");
		Serial.print(httpCode,DEC);
        if (httpCode == 200)
        { //Check is a file was returned

            String payload = http.getString();

            int str_len = payload.length() + 1;
            char JSONMessage[str_len];
            payload.toCharArray(JSONMessage, str_len);
			      Serial.print(JSONMessage);
            StaticJsonDocument<500> JSONDocument; //Memory pool
            DeserializationError err = deserializeJson(JSONDocument, JSONMessage);

            if (err)
            { //Check for errors in parsing
                Serial.println("Parsing failed");
                delay(5000);
                return false;
            }

            const char *pltype = JSONDocument["type"];
            float plversion = JSONDocument["version"];
            const char *plhost = JSONDocument["host"];
            _port = JSONDocument["port"];
            const char *plbin = JSONDocument["bin"];

            String jshost(plhost);
            String jsbin(plbin);

            _host = jshost;
            _bin = jsbin;

            String fwtype(pltype);

            if (plversion != _firwmareVersion && fwtype == _firwmareType)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        else
        {
            Serial.println("Error on HTTP request");
            return false;
        }

        http.end(); //Free the resources
        return false;
    }
    return false;
}

String esp32FOTA::getDeviceID()
{
    char deviceid[21];
    uint64_t chipid;
    chipid = ESP.getEfuseMac();
    sprintf(deviceid, "%" PRIu64, chipid);
    String thisID(deviceid);
    return thisID;
}

// Force a firmware update regartless on current version
void esp32FOTA::forceUpdate(String firmwareHost, int firmwarePort, String firmwarePath)
{
    _host = firmwareHost;
    _bin = firmwarePath;
    _port = firmwarePort;
    execOTA();
}

//=============================================================================
//=======================UPDATE OVER HTTPS=====================================
//=============================================================================

/*
This structure describes the characteristics of a firmware, namely:
the host where it is located
the path of the actual bin file 
the port where it can be retrieved
the type of target device
the version of the firmware
*/
typedef struct
{
    int version;
    int port;
    String host;
    String bin;
    String type;
} OTADescription;

/*
The constructor sets only the firmware type and its versions, as these two 
parameters are hardcoded in the device. The other parameters have to be set 
separately.
*/
secureEsp32FOTA::secureEsp32FOTA(String firwmareType, float firwmareVersion)
{
    _firwmareType = firwmareType;
    _firwmareVersion = firwmareVersion;
}

//---------ARA271121------------------------------------------------------
/*
The constructor sets only the firmware type and its versions, as these two 
parameters are hardcoded in the device. The other parameters have to be set 
separately.
*/
// secureEsp32FOTA::secureEsp32FOTASpecial(String firwmareType, float firwmareVersion)
// {
//     _firwmareType = firwmareType;
//     _firwmareVersion = firwmareVersion;
// }
//---------ARA271121------------------------------------------------------

/*
This function initializes the connection with the server, verifying that it is
compliant with the provided certificate.
*/
bool secureEsp32FOTA::prepareConnection(String destinationServer)
{
    char *certificate = _certificate;
    clientForOta.setCACert(certificate);
    if (clientForOta.connect(destinationServer.c_str(), _securePort))
    {
        return true;
    }
    return false;
}

/*
This function performs a GET request to fetch a resource over HTTPS.
It makes use of the attributes of the class, that have to be already set.
In particular, it makes use of the certificate, the descriptionOfFirmwareURL, 
the host.
*/
String secureEsp32FOTA::secureGetContent()
{
    String destinationURL = _descriptionOfFirmwareURL;
    char *certificate = _certificate;
    noInterrupts();
    bool canConnectToServer = prepareConnection(_host);
    interrupts();
    delay(100);
    if (canConnectToServer)
    {
        //Serial.println("connected");
        clientForOta.println("GET https://" + String(_host) + destinationURL + " HTTP/1.0");
        clientForOta.println("Host: " + String(_host) + "");
        clientForOta.println("Connection: close");
        clientForOta.println();
        
        if(clientForOta.connected())
        {
            Serial.println("clientForOta.connected---------");
        }
        else
        {
            Serial.println("clientForOta.NOT.connected---------");
        }
        while (clientForOta.connected())
        {
            String line = clientForOta.readStringUntil('\n');
            if (line == "\r")
            {
                Serial.println("headers received");
                break;
            }
        }

        String partialContentOfHTTPSResponse = "";
        while (clientForOta.available())
        {
            char c = clientForOta.read();
            partialContentOfHTTPSResponse += c;
            Serial.print(c);
        }
        clientForOta.stop();
        return partialContentOfHTTPSResponse;
    }
    else
    {
        Serial.println("Connection Failed..................!");
    }
    clientForOta.stop();
    return "";
}

/*
This function checks whether it is necessary to perform an OTA update.
It fetches the description of a firmware from the host, reading descriptionOfFirmwareURL.
In case the version of the fetched firmware is greater than the current version, and 
the content has the right format, it returns true, and it modifies the attributes 
of secureEsp32FOTA accordingly.
*/
bool secureEsp32FOTA::execHTTPSCheck()
{
    char *certificate = _certificate;

    String destinationUrl = _descriptionOfFirmwareURL;
    OTADescription obj;
    OTADescription *description = &obj;

    String unparsedDescriptionOfFirmware = secureGetContent();

    int str_len = unparsedDescriptionOfFirmware.length() + 1;
    char JSONMessage[str_len];
    unparsedDescriptionOfFirmware.toCharArray(JSONMessage, str_len);
	// Serial.println(JSONMessage);
    StaticJsonDocument<500> JSONDocument; //Memory pool
    DeserializationError err = deserializeJson(JSONDocument, JSONMessage);
    Serial.println("Current Version No: " + String(_firwmareVersion));
    //Serial.println(_firwmareVersion);

    if (err)
    { //Check for errors in parsing
        Serial.println("Parsing failed.............!");
        delay(500);
        return false;
    }

    description->type = JSONDocument["type"].as<String>();

    description->host = JSONDocument["host"].as<String>();
    description->version = JSONDocument["version"].as<int>();
    description->bin = JSONDocument["bin"].as<String>();

    clientForOta.stop();

    if (description->version > _firwmareVersion && description->type == _firwmareType)
    {
        locationOfFirmware = description->host;
        _bin = description->bin;
        // Serial.println(JSONMessage);
        return true;
    }

    return false;
}

/*--------ARA241121----------------------------------------------------------------------*/
/*
This function checks whether it is necessary to perform an OTA update.
It fetches the description of a firmware from the host, reading descriptionOfFirmwareURL.
In case the version of the fetched firmware is greater than the current version, and 
the content has the right format, it returns true, and it modifies the attributes 
of secureEsp32FOTA accordingly.
*/
uint8_t secureEsp32FOTA::execHTTPSCheckSpecial()
{
    char *certificate = _certificate;

    String destinationUrl = _descriptionOfFirmwareURL;
    OTADescriptionSpecial obj;
    OTADescriptionSpecial *description = &obj;

    String unparsedDescriptionOfFirmware = secureGetContent();

    int str_len = unparsedDescriptionOfFirmware.length() + 1;
    char JSONMessage[str_len];
    unparsedDescriptionOfFirmware.toCharArray(JSONMessage, str_len);
	Serial.println(JSONMessage);
    StaticJsonDocument<500> JSONDocument; //Memory pool
    DeserializationError err = deserializeJson(JSONDocument, JSONMessage);
    Serial.println("Current Version No: " + String(_firwmareVersion));
    //Serial.println(_firwmareVersion);

    if (err)
    { //Check for errors in parsing
        Serial.println("Parsing failed.............!");
        delay(500);
        return false;
    }

    description->type = JSONDocument["type"].as<String>();
    SturOtaJsonReceiv.type = JSONDocument["type"].as<String>();

    description->For = JSONDocument["For"].as<String>();
    SturOtaJsonReceiv.For = JSONDocument["For"].as<String>();

    description->host = JSONDocument["host"].as<String>();
    SturOtaJsonReceiv.host = JSONDocument["host"].as<String>();

    description->version = JSONDocument["version"].as<float>();
    SturOtaJsonReceiv.version = JSONDocument["version"].as<float>();

    description->bin = JSONDocument["bin"].as<String>();
    SturOtaJsonReceiv.bin = JSONDocument["bin"].as<String>();
    Serial.println(JSONMessage);

    clientForOta.stop();

    if (description->version != _firwmareVersion && description->type == _firwmareType)
    {
        //if(description->For == "FW_ALL")
        //{
        locationOfFirmware = description->host;
        _bin = description->bin;
        //Serial.println(JSONMessage);
        return true;
        //}
    }

    return false;
}
/*--------ARA241121----------------------------------------------------------------------*/



/*
This function checks whether the content of a response is a firmware.
*/
bool secureEsp32FOTA::isValidContentType(String contentType)
{
    if (contentType == "application/octet-stream")
    {
        return true;
    }

    return false;
}

/*
This function launches an OTA, using the attributes of the class that describe it:
server, url and certificate.
*/

void secureEsp32FOTA::executeOTA()
{
    char *certificate = _certificate;

    Serial.println("location of fw " + String(locationOfFirmware) + _bin + " HTTP/1.0");

    bool canCorrectlyConnectToServer = prepareConnection(locationOfFirmware);
    int contentLength;
    int contentLengthForPerc = 0;
    bool isValid;
    bool gotHTTPStatus = false;
    //esp_task_wdt_reset();               //ARA191121
    if (canCorrectlyConnectToServer)
    {
        //Serial.println("ok");

        clientForOta.println("GET https://" + String(locationOfFirmware) + _bin + " HTTP/1.0");
        clientForOta.println("Host: " + String(locationOfFirmware) + "");
        clientForOta.println("Connection: close");
        clientForOta.println();

        while (clientForOta.connected())
        {
            
            String header, headerValue;
            // read line till /n
            String line = clientForOta.readStringUntil('\n');
			Serial.println(line);							//ARA221021
            // remove space, to check if the line is end of headers
            line.trim();
            //esp_task_wdt_reset();               //ARA191121
            if (!line.length())
            {
                //headers ended
                break; // and get the OTA started
            }

            if (line.startsWith("HTTP/1.1"))
            {
                if (line.indexOf("200") < 0)
                {
                    //Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                    break;
                }
                gotHTTPStatus = true;
            }

            if (false == gotHTTPStatus)
            {
                continue;
            }

            splitHeader(line, header, headerValue);
            // extract headers here
            // Start with content length
            if (header.equalsIgnoreCase("Content-Length"))
            {
                contentLength = headerValue.toInt();
                contentLengthForPerc = contentLength;
                continue;
            }

            // Next, the content type
            if (header.equalsIgnoreCase("Content-Type"))
            {
                isValid = isValidContentType(headerValue);
            }
            if(bErrorInUpdating == 1)
            {
                break;
            }
        }

        if (contentLength && isValid)
        {
            //Serial.println("beginn");
            bool canBegin = Update.begin(contentLength);
            if (canBegin)
            {
                //Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");

                //size_t written = Update.writeStream(clientForOta);
                size_t written = Update.writeStreamEdited(clientForOta,contentLengthForPerc);
                Serial.println("Written : " + String(written) + " Writting file");
                Serial.println(clientForOta);
                if (written == contentLength)
                {
                    Serial.println("Written : " + String(written) + " successfully");
                }
                else
                {
                    Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                }

                if(written == 0)
                {
                    Serial.println("Update not finished? Something went wrong!");
                    bErrorInUpdating = 1;                   //ARA111121
                }

                if (Update.end())
                {
                    Serial.println("OTA done!");
                    if (Update.isFinished())
                    {
                        Serial.println("Update successfully completed. Rebooting.");
                        bUpdateComplete = 1;
                        //ESP.restart();
                    }
                    else
                    {
                        Serial.println("Update not finished? Something went wrong!");
                        bErrorInUpdating = 1;                   //ARA111121
                    }
                }
                else
                {
                    Serial.println("Error Occurred. Error #: " + String(Update.getError()));
                    bErrorInUpdating = 1;                   //ARA111121
                }
            }
            else
            {
                Serial.println(" could not begin");
                bErrorInUpdating = 1;                   //ARA111121
            }
        }
        else
        {
            Serial.println("Content invalid");
            bErrorInUpdating = 1;                   //ARA111121
        }
    }
    else
    {
        Serial.println("Generic error");
        bErrorInUpdating = 1;                   //ARA111121
    }
}

#endif // esp32fota_edited.cpp ends here............

#if 1 // Updater_Edited.cpp Starts here...................
/***********************************************************************
*This file is from Arduino core file.
*File Path: C:\Users\AppData\Local\Arduino15
*Edited By: AKSHAY ATKAR.
*Date:30-11-2021
***********************************************************************/




#include "Arduino.h"
#include "esp_spi_flash.h"
#include "esp_ota_ops.h"
#include "esp_image_format.h"

float OTATotalWrittenPerc = 0;

static const char * _err2str(uint8_t _error){
    if(_error == UPDATE_ERROR_OK){
        return ("No Error");
    } else if(_error == UPDATE_ERROR_WRITE){
        return ("Flash Write Failed");
    } else if(_error == UPDATE_ERROR_ERASE){
        return ("Flash Erase Failed");
    } else if(_error == UPDATE_ERROR_READ){
        return ("Flash Read Failed");
    } else if(_error == UPDATE_ERROR_SPACE){
        return ("Not Enough Space");
    } else if(_error == UPDATE_ERROR_SIZE){
        return ("Bad Size Given");
    } else if(_error == UPDATE_ERROR_STREAM){
        return ("Stream Read Timeout");
    } else if(_error == UPDATE_ERROR_MD5){
        return ("MD5 Check Failed");
    } else if(_error == UPDATE_ERROR_MAGIC_BYTE){
        return ("Wrong Magic Byte");
    } else if(_error == UPDATE_ERROR_ACTIVATE){
        return ("Could Not Activate The Firmware");
    } else if(_error == UPDATE_ERROR_NO_PARTITION){
        return ("Partition Could Not be Found");
    } else if(_error == UPDATE_ERROR_BAD_ARGUMENT){
        return ("Bad Argument");
    } else if(_error == UPDATE_ERROR_ABORT){
        return ("Aborted");
    }
    return ("UNKNOWN");
}

static bool _partitionIsBootable(const esp_partition_t* partition){
    uint8_t buf[ENCRYPTED_BLOCK_SIZE];
    if(!partition){
        return false;
    }
    if(!ESP.partitionRead(partition, 0, (uint32_t*)buf, ENCRYPTED_BLOCK_SIZE)) {
        return false;
    }

    if(buf[0] != ESP_IMAGE_HEADER_MAGIC) {
        return false;
    }
    return true;
}

bool UpdateClass::_enablePartition(const esp_partition_t* partition){
    if(!partition){
        return false;
    }
    return ESP.partitionWrite(partition, 0, (uint32_t*) _skipBuffer, ENCRYPTED_BLOCK_SIZE);
}

UpdateClass::UpdateClass()
: _error(0)
, _buffer(0)
, _bufferLen(0)
, _size(0)
, _progress_callback(NULL)
, _progress(0)
, _paroffset(0)
, _command(U_FLASH)
, _partition(NULL)
{
}

UpdateClass& UpdateClass::onProgress(THandlerFunction_Progress fn) {
    _progress_callback = fn;
    return *this;
}

void UpdateClass::_reset() {
    if (_buffer)
        delete[] _buffer;
    _buffer = 0;
    _bufferLen = 0;
    _progress = 0;
    _size = 0;
    _command = U_FLASH;

    if(_ledPin != -1) {
      digitalWrite(_ledPin, !_ledOn); // off
    }
}

bool UpdateClass::canRollBack(){
    if(_buffer){ //Update is running
        return false;
    }
    const esp_partition_t* partition = esp_ota_get_next_update_partition(NULL);
    return _partitionIsBootable(partition);
}

bool UpdateClass::rollBack(){
    if(_buffer){ //Update is running
        return false;
    }
    const esp_partition_t* partition = esp_ota_get_next_update_partition(NULL);
    return _partitionIsBootable(partition) && !esp_ota_set_boot_partition(partition);
}

bool UpdateClass::begin(size_t size, int command, int ledPin, uint8_t ledOn, const char *label) {
    if(_size > 0){
        log_w("already running");
        return false;
    }

    _ledPin = ledPin;
    _ledOn = !!ledOn; // 0(LOW) or 1(HIGH)

    _reset();
    _error = 0;
    _target_md5 = emptyString;
    _md5 = MD5Builder();

    if(size == 0) {
        _error = UPDATE_ERROR_SIZE;
        return false;
    }

    if (command == U_FLASH) {
        _partition = esp_ota_get_next_update_partition(NULL);
        if(!_partition){
            _error = UPDATE_ERROR_NO_PARTITION;
            return false;
        }
        log_d("OTA Partition: %s", _partition->label);
    }
    else if (command == U_SPIFFS) {
        _partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, label);
        _paroffset = 0;
        if(!_partition){
            _partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
            _paroffset = 0x1000;  //Offset for ffat, assuming size is already corrected
            if(!_partition){
               _error = UPDATE_ERROR_NO_PARTITION;
               return false;
            }
        }
    }
    else {
        _error = UPDATE_ERROR_BAD_ARGUMENT;
        log_e("bad command %u", command);
        return false;
    }

    if(size == UPDATE_SIZE_UNKNOWN){
        size = _partition->size;
    } else if(size > _partition->size){
        _error = UPDATE_ERROR_SIZE;
        log_e("too large %u > %u", size, _partition->size);
        return false;
    }

    //initialize
    _buffer = (uint8_t*)malloc(SPI_FLASH_SEC_SIZE);
    if(!_buffer){
        log_e("malloc failed");
        return false;
    }
    _size = size;
    _command = command;
    _md5.begin();
    return true;
}

void UpdateClass::_abort(uint8_t err){
    _reset();
    _error = err;
}

void UpdateClass::abort(){
    _abort(UPDATE_ERROR_ABORT);
}

bool UpdateClass::_writeBuffer(){
    //first bytes of new firmware
    uint8_t skip = 0;
    if(!_progress && _command == U_FLASH){
        //check magic
        if(_buffer[0] != ESP_IMAGE_HEADER_MAGIC){
            _abort(UPDATE_ERROR_MAGIC_BYTE);
            return false;
        }

        //Stash the first 16 bytes of data and set the offset so they are
        //not written at this point so that partially written firmware
        //will not be bootable
        skip = ENCRYPTED_BLOCK_SIZE;
        _skipBuffer = (uint8_t*)malloc(skip);
        if(!_skipBuffer){
            log_e("malloc failed");
        return false;
        }
        memcpy(_skipBuffer, _buffer, skip);
    }
    if (!_progress && _progress_callback) {
        _progress_callback(0, _size);
    }
    if(!ESP.partitionEraseRange(_partition, _progress, SPI_FLASH_SEC_SIZE)){
        _abort(UPDATE_ERROR_ERASE);
        return false;
    }
    if (!ESP.partitionWrite(_partition, _progress + skip, (uint32_t*)_buffer + skip/sizeof(uint32_t), _bufferLen - skip)) {
        _abort(UPDATE_ERROR_WRITE);
        return false;
    }
    //restore magic or md5 will fail
    if(!_progress && _command == U_FLASH){
        _buffer[0] = ESP_IMAGE_HEADER_MAGIC;
    }
    _md5.add(_buffer, _bufferLen);
    _progress += _bufferLen;
    _bufferLen = 0;
    if (_progress_callback) {
        _progress_callback(_progress, _size);
    }
    return true;
}

bool UpdateClass::_verifyHeader(uint8_t data) {
    if(_command == U_FLASH) {
        if(data != ESP_IMAGE_HEADER_MAGIC) {
            _abort(UPDATE_ERROR_MAGIC_BYTE);
            return false;
        }
        return true;
    } else if(_command == U_SPIFFS) {
        return true;
    }
    return false;
}

bool UpdateClass::_verifyEnd() {
    if(_command == U_FLASH) {
        if(!_enablePartition(_partition) || !_partitionIsBootable(_partition)) {
            _abort(UPDATE_ERROR_READ);
            return false;
        }

        if(esp_ota_set_boot_partition(_partition)){
            _abort(UPDATE_ERROR_ACTIVATE);
            return false;
        }
        _reset();
        return true;
    } else if(_command == U_SPIFFS) {
        _reset();
        return true;
    }
    return false;
}

bool UpdateClass::setMD5(const char * expected_md5){
    if(strlen(expected_md5) != 32)
    {
        return false;
    }
    _target_md5 = expected_md5;
    return true;
}

bool UpdateClass::end(bool evenIfRemaining){
    if(hasError() || _size == 0){
        return false;
    }

    if(!isFinished() && !evenIfRemaining){
        log_e("premature end: res:%u, pos:%u/%u\n", getError(), progress(), _size);
        _abort(UPDATE_ERROR_ABORT);
        return false;
    }

    if(evenIfRemaining) {
        if(_bufferLen > 0) {
            _writeBuffer();
        }
        _size = progress();
    }

    _md5.calculate();
    if(_target_md5.length()) {
        if(_target_md5 != _md5.toString()){
            _abort(UPDATE_ERROR_MD5);
            return false;
        }
    }

    return _verifyEnd();
}

size_t UpdateClass::write(uint8_t *data, size_t len) {
    if(hasError() || !isRunning()){
        return 0;
    }

    if(len > remaining()){
        _abort(UPDATE_ERROR_SPACE);
        return 0;
    }

    size_t left = len;

    while((_bufferLen + left) > SPI_FLASH_SEC_SIZE) {
        size_t toBuff = SPI_FLASH_SEC_SIZE - _bufferLen;
        memcpy(_buffer + _bufferLen, data + (len - left), toBuff);
        _bufferLen += toBuff;
        if(!_writeBuffer()){
            return len - left;
        }
        left -= toBuff;
    }
    memcpy(_buffer + _bufferLen, data + (len - left), left);
    _bufferLen += left;
    if(_bufferLen == remaining()){
        if(!_writeBuffer()){
            return len - left;
        }
    }
    return len;
}

size_t UpdateClass::writeStream(Stream &data) {
    size_t written = 0;
    size_t toRead = 0;
    int timeout_failures = 0;

    if(hasError() || !isRunning())
        return 0;

    if(!_verifyHeader(data.peek())) {
        _reset();
        return 0;
    }

    if(_ledPin != -1) {
        pinMode(_ledPin, OUTPUT);
    }

    while(remaining()) {
        if(_ledPin != -1) {
            digitalWrite(_ledPin, _ledOn); // Switch LED on
        }
        size_t bytesToRead = SPI_FLASH_SEC_SIZE - _bufferLen;
        if(bytesToRead > remaining()) {
            bytesToRead = remaining();
        }
		
		/* EDITED
        Init read&timeout counters and try to read, if read failed, increase counter,
        wait 100ms and try to read again. If counter > 10 (1 sec), give up/abort
        */
        toRead = 0;
        timeout_failures = 0;
        while(!toRead) {
            toRead = data.readBytes(_buffer + _bufferLen,  bytesToRead);
            Serial.println("Written No of Bytes " + String(toRead));

            if(toRead == 0) {
                timeout_failures++;
				Serial.println("Retry TimeOut " + String(timeout_failures));
                if (timeout_failures >= 10) {   //ARA241121
                    _abort(UPDATE_ERROR_STREAM);
                    return written;
                }
                delay(100);                   //ARA241121
                //vTaskDelay(100);                //ARA241121
            }
        }
//ARA241121------------------------------------------------------------------------------------------------------


        if(_ledPin != -1) {
            digitalWrite(_ledPin, !_ledOn); // Switch LED off
        }
        _bufferLen += toRead;
        if((_bufferLen == remaining() || _bufferLen == SPI_FLASH_SEC_SIZE) && !_writeBuffer())
            return written;
        written += toRead;
    }
    return written;
}

size_t UpdateClass::writeStreamEdited(Stream &data,int &TotalBytes) 
{
    size_t written = 0;
    size_t toRead = 0;
    float TotalWrittenBytes = 0;          //ARA031221
    float TotalNoOfBytes = ((float)TotalBytes);
    int timeout_failures = 0;
         Serial.println("Total No of Bytes to be Written " + String(TotalNoOfBytes));   
    if(hasError() || !isRunning())
        return 0;

    if(!_verifyHeader(data.peek())) {
        _reset();
        return 0;
    }

    if(_ledPin != -1) {
        pinMode(_ledPin, OUTPUT);
    }

    while(remaining()) 
    {
        if(_ledPin != -1) {
            digitalWrite(_ledPin, _ledOn); // Switch LED on
        }
        size_t bytesToRead = SPI_FLASH_SEC_SIZE - _bufferLen;
        if(bytesToRead > remaining()) {
            bytesToRead = remaining();
        }

        toRead = 0;
        timeout_failures = 0;
        while(!toRead) {
            toRead = data.readBytes(_buffer + _bufferLen,  bytesToRead);
            Serial.println("Written No of Bytes " + String(toRead));
            TotalWrittenBytes = TotalWrittenBytes + ((float)toRead);
            OTATotalWrittenPerc = ((TotalWrittenBytes/TotalNoOfBytes)*100);
            Serial.println("Written : " + String(OTATotalWrittenPerc) + " %");
            if(toRead == 0) {
                timeout_failures++;
				Serial.println("Retry TimeOut " + String(timeout_failures));
                if (timeout_failures >= 10) {   //ARA241121
                    _abort(UPDATE_ERROR_STREAM);
                    return written;
                }
                delay(100);                   //ARA241121
                //vTaskDelay(100);                //ARA241121
            }
        }
//ARA241121------------------------------------------------------------------------------------------------------


        if(_ledPin != -1) {
            digitalWrite(_ledPin, !_ledOn); // Switch LED off
        }
        _bufferLen += toRead;
        if((_bufferLen == remaining() || _bufferLen == SPI_FLASH_SEC_SIZE) && !_writeBuffer())
            return written;
        written += toRead;
    }
    return written;
}

//********TRY******ARA031221***************************************
size_t UpdateClass::writeStreamEdited2(Stream &data,int &TotalBytes) 
{
    size_t written;
    size_t toRead;
    float TotalWrittenBytes;          //ARA031221
    float TotalWrittenPerc;
    float TotalNoOfBytes;
    int timeout_failures;
    static int WriterStates;
    bool bInitError;
    switch(WriterStates)
    {
        case WRITE_STATE_INIT :
        {
            written = 0;
            toRead = 0;
            TotalWrittenBytes = 0;          //ARA031221
            TotalWrittenPerc = 0;
            TotalNoOfBytes = ((float)TotalBytes);
            timeout_failures = 0;
            WriterStates = WRITE_STATE_CHECK_ERROR;
        }
        break;

        case WRITE_STATE_CHECK_ERROR :
        {
            Serial.println("Total No of Bytes to be Written " + String(TotalNoOfBytes));   
            if(hasError() || !isRunning())
                bInitError = 1;

            if(!_verifyHeader(data.peek())) {
                _reset();
                bInitError = 1;
            }
            if(bInitError == 1)
            {
                /*******************************/
                return 0;
            }
            else
            {
                WriterStates = WRITE_STATE_REMAINING;
            }
            if(_ledPin != -1) {
                pinMode(_ledPin, OUTPUT);
            }
        }
        break;

        case WRITE_STATE_REMAINING :
        {
            if(remaining()) 
            {
                if(_ledPin != -1) {
                    digitalWrite(_ledPin, _ledOn); // Switch LED on
                }
                size_t bytesToRead = SPI_FLASH_SEC_SIZE - _bufferLen;
                if(bytesToRead > remaining()) {
                    bytesToRead = remaining();
                }

                toRead = 0;
                timeout_failures = 0;
                while(!toRead) {
                    toRead = data.readBytes(_buffer + _bufferLen,  bytesToRead);
                    Serial.println("Written No of Bytes " + String(toRead));
                    TotalWrittenBytes = TotalWrittenBytes + ((float)toRead);
                    TotalWrittenPerc = ((TotalWrittenBytes/TotalNoOfBytes)*100);
                    
                    Serial.println("Written : " + String(TotalWrittenPerc) + " %");
                    if(toRead == 0) {
                        timeout_failures++;
                        Serial.println("Retry TimeOut " + String(timeout_failures));
                        if (timeout_failures >= 10) {   //ARA241121
                            _abort(UPDATE_ERROR_STREAM);
                            return written;
                        }
                        delay(100);                   //ARA241121
                        //vTaskDelay(100);                //ARA241121
                    }
                }
        //ARA241121------------------------------------------------------------------------------------------------------


                if(_ledPin != -1) {
                    digitalWrite(_ledPin, !_ledOn); // Switch LED off
                }
                _bufferLen += toRead;
                if((_bufferLen == remaining() || _bufferLen == SPI_FLASH_SEC_SIZE) && !_writeBuffer())
                    return written;
                written += toRead;
            }
            else
            {
                WriterStates = WRITE_STATE_WRITTEN;
            }
        }
        break;

        case WRITE_STATE_WRITTEN :
        {
            return written;
        }
        break;
    }
}





void UpdateClass::printError(Print &out){
    out.println(_err2str(_error));
}

const char * UpdateClass::errorString(){
    return _err2str(_error);
}

UpdateClass Update;
#endif  // Updater_Edited.cpp Ends here...................


#if 1 // Just call Below 2 functions..........................
/**************************************************************************************************
 * Steps to Add FOTA To Project.....
 * 1. Download and add library/dependencies for fota lib_deps = bblanchon/ArduinoJson@^6.18.5
 * 2. Add C files OTA.cpp and Header files OTA.h to project
 * 3. Call init_Fota( ) function in setup loop
 * 4. Call func_FotaTrigger( ) function in main loop
 * 5. 
****************************************************************************************************/

TaskHandle_t TaskOTA;
void init_Fota(uint8_t Status)
{
    if(Status == 1)
    {
        xTaskCreatePinnedToCore(TaskOTAcode,"TaskOTA",10000,NULL,1,&TaskOTA,1); 
        vTaskSuspend(TaskOTA);        
    }
}

void func_FotaTrigger(uint8_t Status)
{
    if(Status == 1)
    {
        Serial.println("|----------------vTaskResume(TaskOTA)----------------|");
        vTaskResume(TaskOTA);
    }
}
#endif
