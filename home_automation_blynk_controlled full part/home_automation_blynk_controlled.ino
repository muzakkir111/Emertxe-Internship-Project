/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLpfcCHdJt"
#define BLYNK_DEVICE_NAME "HOME AUTOMATION"
#define BLYNK_AUTH_TOKEN "tNR4ueO0vAR2l-oPPHIr4hSFAtmyJZ7b"


// Comment this out to disable prints 
//#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw;
bool inlet_sw,outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
int value = param.asInt();
/*if cooler button is on Blynk mobile application , then turn on the cooler*/
  if (value)
  {
    cooler_control(ON);
    lcd.setCursor(7,0);
    lcd.print("CO_LR ON  ");     
  }
  else
  {
    cooler_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("CO_LR OFF  ");      
  }  
}
  
  

/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN )
{
heater_sw = param.asInt();
/*if heater button is on Blynk mobile application , then turn on the heater*/
  if (heater_sw)
  {
       
    heater_control(ON);
    lcd.setCursor(7,0);
    lcd.print("HT_R ON  ");      
    
    }
  else
  {
    heater_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("HT_R OFF  ");       
  }  
}

  

/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)

 {
  inlet_sw = param.asInt(); 
/*if inlet valve button is at logic high turn on the inlet valve else off*/
 if(inlet_sw)
 {
  enable_inlet();

  lcd.setCursor(7,1);
  lcd.print("IN_FL_ON ");  
   
 }
 else
{
  disable_inlet();

  lcd.setCursor(7,1);
  lcd.print("IN_FL_OFF");
   
} 
 }

/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)

{
 outlet_sw = param.asInt(); 
/*if outlet valve button is at logic high turn on the outlet valve else off*/
 if(outlet_sw)
 {
  enable_outlet();

  lcd.setCursor(7,1);
  lcd.print("OT_FL_ON ");  
   
 }
 else
{
  disable_outlet();

  lcd.setCursor(7,1);
  lcd.print("OT_FL_OFF");
   
} 
  
}  

/* To display temperature and water volume as gauge on the Blynk App*/  
void update_temperature_reading()
{

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  
  Blynk.virtualWrite(TEMPERATURE_GAUGE,read_temperature());
  Blynk.virtualWrite(WATER_VOL_GAUGE, volume());  
  
}


/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
/*compare  temperature with 35 and check if heater is ON*/
if((read_temperature() > float(35)) && heater_sw )
{
/*to turn off the heater*/    
heater_sw =0;
heater_control(OFF);
/*display notification on the CLCD*/
lcd.setCursor(7,0);
lcd.print("HT_R OFF  "); 

/*to display notification on the Blynk*/
Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is above 35 degree celcius\n" );
Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the heater\n" );

/*to reflect OFF on the heater button*/
Blynk.virtualWrite(HEATER_V_PIN, OFF);

}
  
}
  


/*To control water volume above 2000ltrs*/
void handle_tank(void)

 {
  /*compare the volume of water within 2000 ltr and check the status of the inlet valve*/
  if((tank_volume < 2000) && (inlet_sw == OFF))
{
 /*enable inlet valve and print the status on the CLCD*/
  enable_inlet();

  lcd.setCursor(7,1);
  lcd.print("IN_FL_ON ");  
  inlet_sw = ON;

 /*update the inlet button status on the Blynk app as ON*/
  Blynk.virtualWrite(INLET_V_PIN, ON);

  /*display the notification on the virtualterminal*/
Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water volume is less than 2000\n" );
Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning ON the inlet valve\n" );  
     
}  
/*check the tank is full than the turn OFF the inlet valve*/
 /*compare the volume of water within 2000 ltr and check the status of the inlet valve*/
  if((tank_volume == 3000) && (inlet_sw == ON))
{
 /*disable inlet valve and print the status on the CLCD*/
  disable_inlet();

  lcd.setCursor(7,1);
  lcd.print("IN_FL_OFF ");  
  inlet_sw = OFF;

 /*update the inlet button status on the Blynk app as ON*/
  Blynk.virtualWrite(INLET_V_PIN, OFF);

  /*display the notification on the virtualterminal*/
Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water level is full\n" );
Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the inlet valve\n" );  
     

  
}

} 
  




void setup(void)
{
  /*connecting the board to the Blynk server*/
  Blynk.begin(auth);
    /*initialize the lcd*/
    lcd.init();  
    /*turn the backlight */                   
    lcd.backlight();
    /*clear the clcd*/
    lcd.clear();
    /*cursor to the home */
    lcd.home();
  /*initialising garden light as output pin*/
  init_ldr();
/*Initialising garden lights as output pin*/   
 init_temperature_system(); 

 lcd.setCursor(0,0);
 lcd.print("T=");
 
 /*set cursor to second line to display volume of the water*/
  lcd.setCursor(0,1);
  lcd.print("V=");
  /*initialising serial tank*/  
  init_serial_tank();
    


 
/*update temperature on the Blynk app for every .5 second*/ 
 timer.setInterval(500L,update_temperature_reading);

}

void loop(void) 
{
/*controlling the brightness using LDR sensor*/  
 brightness_control(); 
  
 String temperature;
 temperature =String (read_temperature(), 2); 
 /*displaying the temperature on the CLCD*/
 lcd.setCursor(2,0); 
 lcd.print(temperature);

 /*display the volume of the water on the CLCD*/
 
 tank_volume = volume();
lcd.setCursor(2,1); 
lcd.print(tank_volume);
/*to check thresold temperature and controllimg heater*/
handle_temp();
/*to monitor volume of the water and if less 2000ltrs turn on the inlet valve*/
handle_tank();
/*to run the Blynk application*/ 
 Blynk.run();
 timer.run(); 


}
