/*
   esp32 firmware OTA
   Date: December 2018
   Author: Chris Joyce <https://github.com/chrisjoyce911/esp32FOTA/esp32FOTA>
   Purpose: Perform an OTA update from a bin located on a webserver (HTTP Only)
   Edited By: AKSHAY ATKAR
*/

#ifndef esp32fota_Edited_h
#define esp32fota_Edited_h

#include "Arduino.h"
#include <WiFiClientSecure.h>


/****ARA241121***************************************************************************/
#define                 FW_ALL                        1
#define                 FW_ALONE                      2
#define                 FN_NO_UPDATE                  0

/****ARA241121***************************************************************************/


class esp32FOTA
{ 
public:
  esp32FOTA(String firwmareType, float firwmareVersion);      //ARA271121
  //void esp32FOTASpecial(String firwmareType_, float_t firwmareVersion_);       //ARA271121
  void forceUpdate(String firwmareHost, int firwmarePort, String firwmarePath);
  void execOTA();
  void execOTASpecial();               //ARA261121  
  bool execHTTPcheck();
  bool execHTTPcheckSpecial();         //ARA261121
  bool useDeviceID;
  String checkURL;
//----ARA261121-----------------------
  typedef struct
{
    float version;
    int port;
    String host;
    String bin;
    String type;
    String For;
    String Date;
} OTADescriptionSpecialHttp;
  OTADescriptionSpecialHttp SturOtaJsonReceivHttp;
//----ARA261121-----------------------

private:
  String getDeviceID();
  String _firwmareType;
  float _firwmareVersion;
  String _host;
  String _bin;
  int _port;
};

class secureEsp32FOTA
{
public:
  secureEsp32FOTA(String firwmareType, float firwmareVersion);      //ARA271121
  //secureEsp32FOTASpecial(String firwmareType, float firwmareVersion);       //ARA271121
  bool execHTTPSCheck();
  uint8_t execHTTPSCheckSpecial();     //ARA241121
  void executeOTA();
  String _descriptionOfFirmwareURL;
  char *_certificate;
  unsigned int _securePort = 443;
  WiFiClientSecure clientForOta;
  String _host;
//----ARA261121-----------------------
  typedef struct
{
    float version;
    int port;
    String host;
    String bin;
    String type;
    String For;
} OTADescriptionSpecial;
  OTADescriptionSpecial SturOtaJsonReceiv;
  //OTADescriptionSpecial *descriptionOTA = SturOtaJsonReceiv;
//----ARA261121-----------------------

private:
  bool prepareConnection(String locationOfServer);
  String secureGetContent();
  bool isValidContentType(String line);
  String _firwmareType;
  float _firwmareVersion;
  String locationOfFirmware;
  String _bin;
  int _port;
};

#endif
