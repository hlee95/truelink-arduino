/* Code for the arduino on the TrueLink lamp beacon device. */

#include <CurieBLE.h>

#define BEACON_ID 1;

const char BEACON_1_SERVICE[] = "19B10000-E8F2-537E-4F6C-D104768A1214";
const char BEACON_1_CHARACTERISTIC[] = "19B10001-E8F2-537E-4F6C-0104768A1214";
const char BEACON_1_NAME[] = "Beacon1";

const char BEACON_2_SERVICE[] = "57877ea7-250f-4011-876d-85efd2efa0dc";
const char BEACON_2_CHARACTERISTIC[] = "56309e1c-2576-4f24-8392-721d052d08d9";
const char BEACON_2_NAME[] = "Beacon2";

// Keep track of pins.
const int RED_LED = 9;    // Red
const int GREEN_LED = 6;    // Green
const int CAPACITIVE_TOUCH_1 = 8; // Capacitive touch sensors
const int CAPACITIVE_TOUCH_2 = 4;

int beaconIsOn = false; // If true, light does "breathing" effect during next loop.

// Create Bluetooth service for this beacon.
BLEService beaconService(BEACON_2_SERVICE);

// Create beacon characteristic and allow remote device to read and write.
BLECharCharacteristic beaconCharacteristic(BEACON_2_CHARACTERISTIC,
  BLERead | BLEWrite | BLENotify | BLEIndicate | BLEBroadcast);

// Define character meanings for exchanging messages with the app.
const String TURN_BEACON_ON = "a";
const char SEND_LOVE = 'b';

void setup() {
  Serial.begin(9600);

  Serial.println("Starting beacon...");

  // Setup BLE.
  BLE.begin();
  BLE.setLocalName(BEACON_2_NAME);
  BLE.setAdvertisedService(beaconService);
  beaconService.addCharacteristic(beaconCharacteristic);
  
  BLE.addService(beaconService);

  // Assign event handlers for connection and writes.
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);
  beaconCharacteristic.setEventHandler(BLEWritten, beaconCharacteristicUpdated);
  
  // Set an initial value for the characteristic.
  beaconCharacteristic.setValue(0);

  // Start advertising.
  BLE.advertise(); // Use BLE.end() to stop advertising (and possibly disconnect?)

  // Initialize LEDs to 0.
  analogWrite(GREEN_LED, 0);
  analogWrite(RED_LED, 0);

  Serial.println(beaconService.uuid());
  Serial.println(beaconCharacteristic.uuid());

  Serial.println("Beacon active, ready!");
}

void loop() {
  // Poll for BLE events.
  BLE.poll();

  int sensorValue1 = digitalRead(CAPACITIVE_TOUCH_1);
  int sensorValue2 = digitalRead(CAPACITIVE_TOUCH_2);

  // If two hand touch, blink to indicate that love was sent.
  // And, change the beaconCharacteristic to let the server know? Maybe.
  if (sensorValue1 && sensorValue2) {
    Serial.println("two hands");
    twoHandTouchHandler();
  } else if (sensorValue1 ^ sensorValue2) {
    Serial.println("one hand");
    oneHandTouchHandler();
  }

  if (beaconIsOn) {
    animateLightBreathingEffect();
  }
}

// Handler for when the iOS device signals the beacon to light up.
// Only called once per write, not recurring.
void beaconCharacteristicUpdated(BLEDevice central, BLECharacteristic characteristic) {
  String newValue = String(beaconCharacteristic.value());
  Serial.print("Got new beaconCharacteristic value ");
  Serial.println(newValue);

  if (newValue == TURN_BEACON_ON) {
    beaconIsOn = true;
  }
}


// Helper to be called to constantly check if someone is trying to
// turn the beacon off.
void updateBeaconIsOnFlag() {
  // One hand touch should stop animation next round.
  int sensorValue1 = digitalRead(CAPACITIVE_TOUCH_1);
  int sensorValue2 = digitalRead(CAPACITIVE_TOUCH_2);
  if (sensorValue1 ^ sensorValue2) {
    beaconIsOn = false;
  }
}

// Light up the beacon with cool breathing effect.
void animateLightBreathingEffect() {
  // Fade in.
  Serial.println("Animating breathing effect...");
  for (int i = 1; i < 51; i ++) {    
    analogWrite(GREEN_LED, i * 1);
    analogWrite(RED_LED, i * 5);

    updateBeaconIsOnFlag();
    
    // Wait for 30 milliseconds to see the dimming effect.
    delay(30);
  }
 
  // Fade out.
  for (int i = 51 ; i >= 1; i --) {    
    analogWrite(GREEN_LED, i*1);
    analogWrite(RED_LED, i*5);

    updateBeaconIsOnFlag();
    
    // Wait for 30 milliseconds to see the dimming effect.
    delay(30);
  }
  Serial.println("Animating over");
}

// Handler for one hand touch event.
void oneHandTouchHandler() {
  beaconIsOn = false;
}

// Handler for two hand touch event.
void twoHandTouchHandler() {
  // Visual feedback that love was sent.
  for (int i = 1; i < 50; i =i+7) {
    // Sets the value (range from 0 to 255):
    analogWrite(GREEN_LED, i * 1);
    analogWrite(RED_LED, i * 5);
    // Wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  // Fade out from max to min in increments of 5 points.
  for (int i = 51 ; i >= 0; i --) {
    
    // Sets the value (range from 0 to 255):
    analogWrite(GREEN_LED, i*1);
    analogWrite(RED_LED, i*5);

    updateBeaconIsOnFlag();
    
    // Wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
}

// Called when a central connects to this peripheral.
void blePeripheralConnectHandler(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

// Called when a central disconnects from this peripheral.
void blePeripheralDisconnectHandler(BLEDevice central) {
  Serial.print("Disconnect received from central: ");
  Serial.println(central.address());
}

