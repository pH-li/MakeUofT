/*********************************************************************************************************************************************** 
 
                                                      == PROJECT - Baymax Productivity Vending Machine ==          

 - This program runs the hardware for the Baymax
 - Team members: Aruthy Sriketharan, Lily Phan, Anupam Sahoo, Saheed Quadri.
 - Date: January 17, 2023
 ************************************************************************************************************************************************/

#include <Adafruit_Fingerprint.h>
#include <ArduinoSound.h>
#include <Adafruit_ZeroI2S.h>
#include <elapsedMillis.h>
#include <Stepper.h>
#include <ArduinoBLE.h>

/* Project Components:
 - 2 Stepper motors
 - Passibve Infrared Radiation Sensor
 - Force Sensor
 - Fingerprint Sensor
 - Speaker
*/

const char* bayMax_PVM_Serv_UUID = "e0924458-c730-45f9-a395-b6cf93c4be5a";// Randomly generated UUID using online generator

// Recieve UUID
const char* UUID_vendState= "e0924461-c730-45f9-a395-b6cf93c4be5a"; //Debugging the state of vending snacks

BLEService bayMax_PVM(bayMax_PVM_Serv_UUID); // Create a service for BLE data transfer
//Recieve Characteristics
BLEBoolCharacteristic  dispense(UUID_vendState,  BLEWrite);

// Pinout Configuration
const int forceSensor = 15;
const int PirPin = 17;
const int btnPin = 16;

//Initializing Object Variables
SoftwareSerial fingerSerial(2, 3);
Adafruit_Fingerprint authSense(); // Software serial
Adafruit_ZeroI2S speaker(); // Pins A3, A4, and D4
Stepper Stepper1(stepsPerRevolution, 5,7,8,6);
Stepper Stepper2(stepsPerRevolution, 9,11,10,12);

//Global constants
const int stepsPerRevolution = 200;

Void configPinout()
{
  pinMode(forceSensor, INPUT);
  pinMode(PirPin, INPUT);
  pinMode(btnPin, INPUT);
  Stepper1.setSpeed(80);
  Stepper1.setSpeed(80);
}

void setup()
{
  // Begin Serial
  Serial.begin(115200);
  while (!Serial);

  configPinout();

  // init BLE
  if (!BLE.begin()) 
    {
    Serial.println("BLE: failed");
    }
  Serial.println("BLE: ok");

  // BLE service
  // correct sequence:
  // set BLE name > advertised service > add characteristics > add service > set initial values > advertise

  // Set BLE name
  BLE.setLocalName("Baymax Productivity Vending Machine");
  BLE.setDeviceName("Arduino"); // Arduino is the default value on this module
  
  // Set advertised Service
  BLE.setAdvertisedService(bayMax_PVM);
  
  // Add characteristics to the Service
  bayMax_PVM.addCharacteristic(dispense);
  
  // add service to BLE
  BLE.addService(bayMax_PVM);
 
  // start advertising
  BLE.advertise();
  Serial.println("Advertising started");

 }

bool dispenseSnack ();

void loop()
{

  static long preMillis = 0;
  
  // listen for BLE centrals devices
  BLEDevice central = BLE.central();

  // central device connected?
  if (central) 
    {
    digitalWrite(LED_BUILTIN, HIGH); // turn on the onboard led
    Serial.print("Connected to central: ");
    Serial.println(central.address()); // central device MAC address
    
    // while the central is still connected to peripheral:
    while (central.connected()) 
      {
      // additional placeholder for writing command from central
      // to this device. "myCharacteristic" is a characteristic initialized
      // in write mode (BLEwrite) with his own UUid
      /*
      if (myCharacteristic.written()) 
        {
        command = myCharacteristic.value(); // retrieve value sent from central
        Serial.print(F("commmand value:  "));
        Serial.println(command);
        }
      */
      
      long curMillis = millis();
      if (preMillis>curMillis) preMillis=0; // millis() rollover?
      if (curMillis - preMillis >= 10) // check values every 10mS
        {
        preMillis = curMillis;
        updateValues(); // call function for updating value to send to central
        }
      } // still here while central connected

    // central disconnected:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    } // no central

  }

}

int step1Spd = 2, step2Spd = 3;

void dispenseSnack ()
{
  if(authenticated && (dispense.read(1) || digitalRead(btnPin) == 1 ))
  {
    stepper1.step(stepsPerRevolution/step1Spd);
    delay(500);
    stepper1.step(-(stepsPerRevolution/step1Spd));
    delay(100);
    stepper2.step(stepsPerRevolution/step2Spd);
    delay(500);
    stepper2.step(-(stepsPerRevolution/step2Spd));
    delay(100);
  }
  while(digitalRead(forcePin)  != 1);
  dispensed = true;
}
 

