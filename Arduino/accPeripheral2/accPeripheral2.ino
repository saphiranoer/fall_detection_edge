/*
 * Device: Arduino Nano 33 BLE Sense
 * Peripheral
 * The values of the integrated temperature sensor and 
 * accelerometer are sent using BLE.
 */

#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h> //accelerometer sensor
#include <math.h>

int const d_a=2; //number of decimal to keep for the accelerometer

// Calibration parameters
float xOffset = 0.0;
float yOffset = 0.0;
float zOffset = 0.0;
float scaleFactor = 1.0;

// Filtering parameters
float alpha = 0.2;
float filteredX = 0.0;
float filteredY = 0.0;
float filteredZ = 0.0;

//float values read by the sensor
float xSensor=0;
float ySensor=0;
float zSensor=0;

//integer variables to send via BLE
int xBLE=xSensor*pow(10,d_a);
int yBLE=ySensor*pow(10,d_a);
int zBLE=zSensor*pow(10,d_a);

const int numReadings = 50; // Number of readings to collect
float sensorReadings[numReadings]; // Array to store the sensor readings
int currentIndex = 0;

int pinBuzzer= 10;

BLEService SensorService("1101");
BLEUnsignedIntCharacteristic XChar("2101", BLERead | BLENotify);
BLEUnsignedIntCharacteristic YChar("2102", BLERead | BLENotify);
BLEUnsignedIntCharacteristic ZChar("2103", BLERead | BLENotify);

void setup() {
  
  pinMode(pinBuzzer, OUTPUT);
  IMU.begin();
  Serial.begin(9600); 
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("BLE failed to Initiate");
    delay(500);
    while (1);
  }

  BLE.setLocalName("Arduino (peripheral)");
  BLE.setAdvertisedService(SensorService);
  SensorService.addCharacteristic(XChar);
  SensorService.addCharacteristic(YChar);
  SensorService.addCharacteristic(ZChar);
  BLE.addService(SensorService);

  XChar.writeValue(xBLE);
  YChar.writeValue(yBLE);
  ZChar.writeValue(zBLE);

  BLE.advertise();

  calibrateAccelerometer();

  Serial.println("Arduino peripheral device is now active, waiting for connections...");
}

// Function to preprocess accelerometer data
float filterAccelerometerReadings(float& x, float& y, float& z) {
  
  filteredX = alpha * x + (1 - alpha) * filteredX;
  filteredY = alpha * y + (1 - alpha) * filteredY;
  filteredZ = alpha * z + (1 - alpha) * filteredZ;
  x = filteredX;
  y = filteredY;
  z = filteredZ;
  
  float accelerationMagnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));

  return accelerationMagnitude;
}

void calibrateAccelerometer() {
  // Perform static calibration by taking average readings when the Arduino is stationary
  int numSamples = 1000;
  float sumX = 0.0;
  float sumY = 0.0;
  float sumZ = 0.0;

  for (int i = 0; i < numSamples; i++) {
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(xSensor, ySensor, zSensor);
      sumX += xSensor;
      sumY += ySensor;
      sumZ += zSensor;
    }
    delay(1);
  }

  xOffset = sumX / numSamples;
  yOffset = sumY / numSamples;
  zOffset = sumZ / numSamples;
}

void loop() {

  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    digitalWrite(LED_BUILTIN, HIGH);

    if (central.connect()) {
        Serial.println("Connected");
    }
    else {
        Serial.println("Failed to connect!");
        return;
    }

    // Discover peripheral attributes
    Serial.println("Discovering attributes ...");
    if (central.discoverAttributes()) {
        Serial.println("Attributes discovered");
    }
    else {
        Serial.println("Attribute discovery failed!");
        central.disconnect();
        return;
    }

    while (central.connected()) {
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(xSensor, ySensor, zSensor);
      }

      // Apply calibration
      xSensor -= xOffset;
      ySensor -= yOffset;
      zSensor -= zOffset;

      // Calculate magnitude
      float magnitude = filterAccelerometerReadings(xSensor, ySensor, zSensor);

      sensorReadings[currentIndex] = magnitude;
  
      currentIndex = (currentIndex + 1) % numReadings;

      Serial.print("x: ");
      Serial.print(xSensor);
      Serial.print(", y: ");
      Serial.print(ySensor);
      Serial.print(", z: ");
      Serial.print(zSensor);
      Serial.println("");

      xBLE=magnitude*pow(10,d_a);
      yBLE=ySensor*pow(10,d_a);
      zBLE=zSensor*pow(10,d_a);

      Serial.print("magnitude: ");
      Serial.print(xBLE);
      Serial.println("");
      // Serial.print(" - yBLE ");
      // Serial.print(yBLE);
      // Serial.print(" - zBLE ");
      // Serial.print(zBLE);
      // Serial.println("");

      XChar.writeValue(xBLE);
      YChar.writeValue(yBLE);
      ZChar.writeValue(zBLE);
      // Serial.print("XChar ");
      // Serial.print(XChar.value());
      // Serial.print(" - YChar ");
      // Serial.print(YChar.value());
      // Serial.print(" - ZChar ");
      // Serial.print(ZChar);
      // Serial.println("");
      // Serial.println("");

      BLEService service = central.service("1102");
      BLECharacteristic FallDetect = service.characteristic("3101");

      if(FallDetect.canRead()){
        int32_t fd = 0;
        FallDetect.readValue(&fd, 4);

        int fdInt = (int)fd;

        if(fdInt == 1){
          Serial.println("Fall");
          tone(pinBuzzer, 3000);

          delay(1000);

          // Stop the sound
          noTone(pinBuzzer);

          delay(1000);
        }
        else{
          Serial.println(" ");
        }
      }

      delay(20);

    }
  }
  else {
    delay(1000);
  }
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
  BLE.advertise();
}
