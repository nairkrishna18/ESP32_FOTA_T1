#include <Arduino.h>

#ifndef _OTA_H_
  #define _OTA_H_
  #include "OTA.h"
#endif



void setup()
{
  Serial.begin(115200);
   
  Serial.println("\n|----------------System-Initializing----------------|");
  Serial.println("Serial Port Configured @115200");
  Serial.printf("VERSION_NO = %d\n",VERSION_NO);
  pinMode(SW3_PIN , INPUT_PULLUP); 
  delay(100);  

#if 1
init_Fota(1);
#endif

  
  Serial.print("\nInitializing.");
  
  for(int i1=0; i1<25; i1++)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\n\n|----------------System-Initialized----------------|\n\n");
  delay(2000); 
}

void loop()
{
  TickType_t xLastWakeUpTime;
  while(1)
  {
    Serial.printf("FOTA_TRIGGER_STATUS = %d\n",digitalRead(SW3_PIN));
    if(FOTA_TRIGGER_DETECTED)
    {
      vTaskDelay(pdMS_TO_TICKS(2000));
      if(FOTA_TRIGGER_DETECTED)
      {
        #if 1
        func_FotaTrigger(1);        
        #endif

      }
    }
    // vTaskDelay(pdMS_TO_TICKS(1000));
    vTaskDelayUntil(&xLastWakeUpTime,pdMS_TO_TICKS(1000));  
    
  }
}
