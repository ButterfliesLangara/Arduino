#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

float acc_x, acc_y, acc_z = 0;
float x, y, z;
float threshold = 6.5;
float xval, yval, zval= 0;
int steps, flag = 0;
float mag, magDelta, magPrevious = 0;

BLEService AccelerometerService("183E"); // create service

// create characteristic and allow remote device to read and write
BLEIntCharacteristic SensorData("2713", BLERead | BLENotify);

const int ledPin = LED_BUILTIN; // pin to use for the LED
void setup() {
  Serial.begin(9600);
//  while (!Serial);
  
  pinMode(ledPin, OUTPUT); // use the LED pin as an output

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("Stepaw");
  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(AccelerometerService);

  // add the characteristic to the service
  AccelerometerService.addCharacteristic(SensorData);

  // add service
  BLE.addService(AccelerometerService);

  // set an initial value for the characteristic
  SensorData.setValue(0);

  // start advertising
  BLE.advertise();

  Serial.println(("Bluetooth device active, waiting for connections..."));
}

void loop() {
  BLEDevice central = BLE.central();
  float x, y, z;
  float xaccl, yaccl, zaccl = 0;
  if(central) {
    Serial.print("Connected to Central: ");
    Serial.println(central.address());
    while(central.connected()) {
      // check if acceleration is available
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(acc_x, acc_y, acc_z);
        xaccl = float(acc_x * 9.8);
        delay(1);
        yaccl = float(acc_y * 9.8);
        delay(1);
        zaccl = float(acc_z * 9.8);
        delay(1);
        // calculate the acceleration vector
        mag = sqrt((xaccl * xaccl) + (yaccl * yaccl) + (zaccl * zaccl));
        magDelta = (mag - magPrevious);
        magPrevious = mag;

        if (magDelta > threshold && flag == 0)
        {
          steps = steps + 1;
          Serial.println('\n');
          Serial.print("steps: ");
          Serial.println('\t');
          Serial.println(steps);
          SensorData.writeValue(steps);
        }
      
        else if(magDelta < threshold && flag == 1) {
          //Don't Count
        }
      
        if(magDelta < threshold && flag == 1) {
          flag = 0;
        }
      
        if (steps < 0) {
          steps = 0;
        }
      } 
    }
  }
}