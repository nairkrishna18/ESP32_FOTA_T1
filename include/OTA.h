#if 1 // OTA.h Starts Here..........
#include "ArduinoJson.h"
#include <WiFiMulti.h>


// #ifndef ESP32FOTA_EDITED_H
//     #define ESP32FOTA_EDITED_H
//     #include "esp32fota_Edited.h"
// #endif

#include <WiFiClientSecure.h>


#define             VERSION_NO                          3




#define                 INIT_WIFI                           0
#define                 WAIT_FOR_CONNECT                    1
#define                 CHECK_FOR_UPDATE_MESSAGE            2
#define                 INIT_FOR_UPDATE                     3
#define                 CHECK_VERSION                       4
#define                 START_OTA                           5
#define                 REBOOT_SYSTEM                       6

// #define JSON_ADDRESS  "/nairkrishna18/PLENT_MASTER/main/CG_PLENT_MASTER_FW_DETAILS.json" // Original Address....
// #define JSON_ADDRESS  "/nairkrishna18/ESP32_FOTA_T1/blob/master/FotaFolder/CG_PLENT_MASTER_FW_DETAILS.json"
#define JSON_ADDRESS  "/nairkrishna18/ESP32_FOTA_T1/master/FotaFolder/ESP32_FOTA_FW_DETAILS.json" // removed blob

void TaskOTAcode( void * parameter );
void FnOtaStates(void);

#endif // OTA.h Ends here..............

#if 1 // esp32fota_Edited.h Starts here.............
/*
   esp32 firmware OTA
   Date: December 2018
   Author: Chris Joyce <https://github.com/chrisjoyce911/esp32FOTA/esp32FOTA>
   Purpose: Perform an OTA update from a bin located on a webserver (HTTP Only)
   Edited By: AKSHAY ATKAR
*/

#ifndef esp32fota_Edited_h
  #define esp32fota_Edited_h
#endif

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



#endif  // esp32fota_Edited.h Ends here.............

#if 1 // Update_Edited.h Starts here.............
/***********************************************************************
*This file is from Arduino core file.
*File Path: C:\Users\AppData\Local\Arduino15
*Edited By: AKSHAY ATKAR.
*Date:30-11-2021
***********************************************************************/
#ifndef ESP8266UPDATER_Edited_H
#define ESP8266UPDATER_Edited_H

#include <Arduino.h>
#include <MD5Builder.h>
#include <functional>
#include "esp_partition.h"

#define UPDATE_ERROR_OK                 (0)
#define UPDATE_ERROR_WRITE              (1)
#define UPDATE_ERROR_ERASE              (2)
#define UPDATE_ERROR_READ               (3)
#define UPDATE_ERROR_SPACE              (4)
#define UPDATE_ERROR_SIZE               (5)
#define UPDATE_ERROR_STREAM             (6)
#define UPDATE_ERROR_MD5                (7)
#define UPDATE_ERROR_MAGIC_BYTE         (8)
#define UPDATE_ERROR_ACTIVATE           (9)
#define UPDATE_ERROR_NO_PARTITION       (10)
#define UPDATE_ERROR_BAD_ARGUMENT       (11)
#define UPDATE_ERROR_ABORT              (12)

#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF

#define U_FLASH   0
#define U_SPIFFS  100
#define U_AUTH    200

#define ENCRYPTED_BLOCK_SIZE 16

//********ARA031221**********************
#define          WRITE_STATE_INIT                         (0)
#define          WRITE_STATE_CHECK_ERROR                  (1)
#define          WRITE_STATE_REMAINING                    (2)
#define          WRITE_STATE_WRITTEN                      (3)



class UpdateClass {
  public:
    typedef std::function<void(size_t, size_t)> THandlerFunction_Progress;

    UpdateClass();

    /*
      This callback will be called when Update is receiving data
    */
    UpdateClass& onProgress(THandlerFunction_Progress fn);

    /*
      Call this to check the space needed for the update
      Will return false if there is not enough space
    */
    bool begin(size_t size=UPDATE_SIZE_UNKNOWN, int command = U_FLASH, int ledPin = -1, uint8_t ledOn = LOW, const char *label = NULL);

    /*
      Writes a buffer to the flash and increments the address
      Returns the amount written
    */
    size_t write(uint8_t *data, size_t len);

    /*
      Writes the remaining bytes from the Stream to the flash
      Uses readBytes() and sets UPDATE_ERROR_STREAM on timeout
      Returns the bytes written
      Should be equal to the remaining bytes when called
      Usable for slow streams like Serial
    */
    size_t writeStream(Stream &data);

    /*---------------------------ARA031221-----
      Writes the remaining bytes from the Stream to the flash
      Uses readBytes() and sets UPDATE_ERROR_STREAM on timeout
      Returns the bytes written
      Should be equal to the remaining bytes when called
      Usable for slow streams like Serial
      take total no of bytes(TotalBytes) to be written to calculate percentage***
    */
    size_t writeStreamEdited(Stream &data, int &TotalBytes);
    
    /*---------------------------ARA031221-----
      Writes the remaining bytes from the Stream to the flash
      Uses readBytes() and sets UPDATE_ERROR_STREAM on timeout
      Returns the bytes written
      Should be equal to the remaining bytes when called
      Usable for slow streams like Serial
      take total no of bytes(TotalBytes) to be written to calculate percentage***
    */
    size_t writeStreamEdited2(Stream &data, int &TotalBytes);

    /*
      If all bytes are written
      this call will write the config to eboot
      and return true
      If there is already an update running but is not finished and !evenIfRemaining
      or there is an error
      this will clear everything and return false
      the last error is available through getError()
      evenIfRemaining is helpfull when you update without knowing the final size first
    */
    bool end(bool evenIfRemaining = false);

    /*
      Aborts the running update
    */
    void abort();

    /*
      Prints the last error to an output stream
    */
    void printError(Print &out);

    const char * errorString();

    /*
      sets the expected MD5 for the firmware (hexString)
    */
    bool setMD5(const char * expected_md5);

    /*
      returns the MD5 String of the successfully ended firmware
    */
    String md5String(void){ return _md5.toString(); }

    /*
      populated the result with the md5 bytes of the successfully ended firmware
    */
    void md5(uint8_t * result){ return _md5.getBytes(result); }

    //Helpers
    uint8_t getError(){ return _error; }
    void clearError(){ _error = UPDATE_ERROR_OK; }
    bool hasError(){ return _error != UPDATE_ERROR_OK; }
    bool isRunning(){ return _size > 0; }
    bool isFinished(){ return _progress == _size; }
    size_t size(){ return _size; }
    size_t progress(){ return _progress; }
    size_t remaining(){ return _size - _progress; }

    /*
      Template to write from objects that expose
      available() and read(uint8_t*, size_t) methods
      faster than the writeStream method
      writes only what is available
    */
    template<typename T>
    size_t write(T &data){
      size_t written = 0;
      if (hasError() || !isRunning())
        return 0;

      size_t available = data.available();
      while(available) {
        if(_bufferLen + available > remaining()){
          available = remaining() - _bufferLen;
        }
        #if 0 // Only for testing --> not working............
        if(_bufferLen + available > 9600) 
        {
          size_t toBuff = 9600 - _bufferLen;
          data.read(_buffer + _bufferLen, toBuff);
          _bufferLen += toBuff;
          if(!_writeBuffer())
            return written;
          written += toBuff;
        }
        #else
        if(_bufferLen + available > 4096) 
        {
          size_t toBuff = 4096 - _bufferLen;
          data.read(_buffer + _bufferLen, toBuff);
          _bufferLen += toBuff;
          if(!_writeBuffer())
            return written;
          written += toBuff;
        } 
        #endif
        else 
        {
          data.read(_buffer + _bufferLen, available);
          _bufferLen += available;
          written += available;
          if(_bufferLen == remaining()) {
            if(!_writeBuffer()) {
              return written;
            }
          }
        }
        if(remaining() == 0)
          return written;
        available = data.available();
      }
      return written;
    }

    /*
      check if there is a firmware on the other OTA partition that you can bootinto
    */
    bool canRollBack();
    /*
      set the other OTA partition as bootable (reboot to enable)
    */
    bool rollBack();

  private:
    void _reset();
    void _abort(uint8_t err);
    bool _writeBuffer();
    bool _verifyHeader(uint8_t data);
    bool _verifyEnd();
    bool _enablePartition(const esp_partition_t* partition);


    uint8_t _error;
    uint8_t *_buffer;
    uint8_t *_skipBuffer;
    size_t _bufferLen;
    size_t _size;
    THandlerFunction_Progress _progress_callback;
    uint32_t _progress;
    uint32_t _paroffset;
    uint32_t _command;
    const esp_partition_t* _partition;

    String _target_md5;
    MD5Builder _md5;

    int _ledPin;
    uint8_t _ledOn;
};

extern UpdateClass Update;

#endif
#endif // Update_Edited.h Ends here................
