#define BLYNK_TEMPLATE_ID "TMPLpfcCHdJt"
#define BLYNK_DEVICE_NAME "HOME AUTOMATION"
#define BLYNK_AUTH_TOKEN "tNR4ueO0vAR2l-oPPHIr4hSFAtmyJZ7b"

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>

#define LED     2


char auth[] = BLYNK_AUTH_TOKEN;


BLYNK_WRITE(V0)
{
  int value = param.asInt();
  if (value)
  {
    digitalWrite(LED,1);
  }
  else
  {
    digitalWrite(LED,0);
  }
  
}

void setup() 
{
  pinMode(LED, OUTPUT);
  // put your setup code here, to run once:
  Blynk.begin(auth);
}

void loop() 
{
  // put your main code here, to run repeatedly:
   Blynk.run();

}
