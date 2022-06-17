#include "ArduinoJson.h"
#include <WiFiMulti.h>


// #include "esp32fota.h"
#include "esp32fota_Edited.h"
#include <WiFiClientSecure.h>


#define             VERSION_NO                          3




#define                 INIT_WIFI                           0
#define                 WAIT_FOR_CONNECT                    1
#define                 CHECK_FOR_UPDATE_MESSAGE            2
#define                 INIT_FOR_UPDATE                     3
#define                 CHECK_VERSION                       4
#define                 START_OTA                           5
#define                 REBOOT_SYSTEM                       6

#define JSON_ADDRESS  "/nairkrishna18/PLENT_MASTER/main/CG_PLENT_MASTER_FW_DETAILS.json"

void TaskOTAcode( void * parameter );
void FnOtaStates(void);