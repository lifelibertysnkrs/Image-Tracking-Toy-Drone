/******************************************************************
Developed by Rahul Tiwari and Gary Chien

Arduino Code and Hardware Schematics based on LeapCopter by TomerN

*******************************************************************/
/*   INCLUDES                                                     */
#include <WString.h> //Official Arduino string library
#include <Wire.h>
#include <Adafruit_MCP4725.h>

/******************************************************************/
/*   CONSTANTS                                                    */
Adafruit_MCP4725 dac;

int DAC_Pins[3] = {8, 10, 12};
int flip_Pin = 5;
#define SERIAL_BAUDRATE 115200            //Baud-Rate of the serial Port

void DAC_Write(uint16_t volt_level, int pin)
{
  digitalWrite(DAC_Pins[pin], 1);
  dac.setVoltage(volt_level, false);
  digitalWrite(DAC_Pins[pin], 0);  
}

/*   METHODS    
******************************************************************
The setup method is executed once after the bootloader is done
with his job.
******************************************************************/
void setup(){

  // Activate all Digital pins on my board
  for (int i = 0; i < 3; i++) {
      pinMode(DAC_Pins[i], OUTPUT);
      digitalWrite(DAC_Pins[i], 0);
  }
  pinMode(flip_Pin, OUTPUT);
  digitalWrite(flip_Pin, 1);
  
  dac.begin(0x61);
  
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Connected");
}

/*****************************************************************
The loop method is executed forever right after the setup method
is finished.
******************************************************************/


void loop(){

  int throttle=0, pitch=2048, roll=2048, flip=0;
  
   while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    throttle = Serial.parseInt(); 
    // do it again:
    pitch = Serial.parseInt(); 
    // do it again:
    roll = Serial.parseInt(); 
    // do it again:
    flip = Serial.parseInt(); 

    // look for the newline. That's the end of your
    // sentence:
    if (Serial.read() == '\n') {

      DAC_Write(throttle, 0);
      DAC_Write(pitch, 1);
      DAC_Write(roll, 2);
      
      digitalWrite(flip_Pin, flip);

    }
  }
}
