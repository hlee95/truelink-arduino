/* Code for the arduino on the TrueLink lamp beacon device. */

#include <CurieBLE.h>

// Either beacon 1 or beacon 2.
#define BEACON_ID 1

// Keep track of pins.
// TODO: make this correct.
#define CAPACITIVE_TOUCH_PIN_1 4
#define CAPACITIVE_TOUCH_PIN_2 5
#define LED_PIN LED_BUILTIN

// If necessary, hardcode WiFi information.
const String WIFI_NETWORK_NAME = "Hanna's_iPhone";
const String WIFI_PASSWORD = "blahblahblah";

// Create Bluetooth service for this beacon.
BLEService beaconService("19B10000-E8F2-537E-4F6C-D104768A1214");

// Create beacon characteristic and allow remote device to read and write.
BLECharCharacteristic beaconCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214",
  BLERead | BLEWrite | BLENotify | BLEIndicate | BLEBroadcast);

// Define character meanings for exchanging messages with the app.
const char TURN_BEACON_1_ON = 'a';
const char TURN_BEACON_2_ON = 'b';
const char SEND_LOVE_TO_BEACON_1 = 'c';
const char SEND_LOVE_TO_BEACON_2 = 'd';

int ledValue = 0;

void setup() {
  Serial.begin(9600);

  // Setup digital pins.
  pinMode(CAPACITIVE_TOUCH_PIN_1, INPUT);
  pinMode(CAPACITIVE_TOUCH_PIN_2, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Setup BLE.
  BLE.begin();
  BLE.setLocalName("Lamp1");
  BLE.setAdvertisedService(beaconService);
  beaconService.addCharacteristic(beaconCharacteristic);
  BLE.addService(beaconService);

  // Assign event handlers for connection and writes.
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  beaconCharacteristic.setEventHandler(BLEWritten, beaconCharacteristicUpdated);
  
  // Set an initial value for the characteristic.
  beaconCharacteristic.setValue(0);

  // Start advertising.
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // Poll for BLE events.
  BLE.poll();

  // Check for capacitive touch to indicate sending a value.
  if (ledValue == 1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

// Returns true if there is currently one touch on the lamp.
bool oneTouch() {
  return false;
}


// Called when a central connects to this peripheral.
void blePeripheralConnectHandler(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

int i = 0;
// Handler for when the iOS device signals the beacon to light up.
void beaconCharacteristicUpdated(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("Got new beaconCharacteristic value ");
  Serial.println(String(beaconCharacteristic.value()));

  if (beaconCharacteristic.value() == TURN_BEACON_1_ON) {
    Serial.println("Turning beacon on");
    ledValue = 1;
  } else {
    Serial.println("Turning beacon off");
    ledValue = 0;
  }

  if (i == 0) {
    beaconCharacteristic.setValue(SEND_LOVE_TO_BEACON_1);
  } else if (i == 1) {
    beaconCharacteristic.setValue(SEND_LOVE_TO_BEACON_2);
  }

  i += 1;
}

