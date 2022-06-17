#include <Arduino.h>
#include "OTA.h"

#define SW3_PIN                   (34)
#define FOTA_TRIGGER_DETECTED     (digitalRead(SW3_PIN))
#define FOTA_TRIGGER_CLEARED      (!digitalRead(SW3_PIN))

TaskHandle_t TaskOTA;

void setup()
{
  pinMode(SW3_PIN , INPUT_PULLUP); 
  delay(100); 
  Serial.begin(115200);
  Serial.println("\n|----------------System-Initializing----------------|");
  Serial.println("Serial Port Configured @115200");
  Serial.printf("VERSION_NO = %d\n",VERSION_NO);
  delay(100); 
  xTaskCreatePinnedToCore(TaskOTAcode,"TaskOTA",10000,NULL,1,&TaskOTA,1); 
  vTaskSuspend(TaskOTA);
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
  while(1)
  {
    Serial.printf("FOTA_TRIGGER_STATUS = %d\n",digitalRead(SW3_PIN));
    if(FOTA_TRIGGER_DETECTED)
    {
      delay(2000);
      if(FOTA_TRIGGER_DETECTED)
      {
        Serial.println("|----------------vTaskResume(TaskOTA)----------------|");
        vTaskResume(TaskOTA);
      }
    }
    delay(1000);
  }
}
