/* Testing that onboard BLE can communicate with iOS app. */

#include <CurieBLE.h>

// Create Bluetooth service.
BLEService beaconService("19B10000-E8F2-537E-4F6C-D104768A1214");

// Create beacon characteristic and allow remote device to read and write
BLECharCharacteristic beaconCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify | BLEIndicate | BLEBroadcast);

void setup() {
  Serial.begin(9600);

  BLE.begin();

  BLE.setLocalName("Lamp1");
  BLE.setAdvertisedService(beaconService);
  
  beaconService.addCharacteristic(beaconCharacteristic);

  // add service
  BLE.addService(beaconService);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);

  // assign event handlers for characteristic
  beaconCharacteristic.setEventHandler(BLEWritten, beaconCharacteristicUpdated);
  
  // set an initial value for the characteristic
  beaconCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // poll for BLE events
  BLE.poll();
}

//void sendString(BLECharacteristic characteristic, String string) {
//  for (int i = 0; i < 5; i++) {
//    beaconCharacteristic.setValue(string[i]);
//    Serial.print("sent value " + String(beaconCharacteristic.value()) + " \n"); 
//  }
//}

// Called when a central connects to us.
void blePeripheralConnectHandler(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
  // Test sending data.
}

String test = "abcdefghijklmnopqrstuvwxyz";
int i = 0;
void beaconCharacteristicUpdated(BLEDevice central, BLECharacteristic characteristic) {
  String newValue = String(beaconCharacteristic.value());
  Serial.println("Got value " + newValue);

  beaconCharacteristic.setValue(test[i]);
  i += 1;

  if (newValue == "a") {
    Serial.println("LED on");
    digitalWrite(LED_BUILTIN, HIGH);
//    delay(1000);
  } else {
    Serial.println("LED off");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

