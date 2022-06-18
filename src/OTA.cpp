#include "OTA.h"


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

// const char* ssid     = "TROJAN_TP2.4";     // your network SSID (name of wifi network)
// const char* password = "9833110550"; // your network password
const char* ssid     = "CG-PLENT_0002";     // your network SSID (name of wifi network)
const char* password = "1234567890"; // your network password

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