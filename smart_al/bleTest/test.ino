#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

bool deviceConnected = false;
bool oldDeviceConnected = false;
//uint32_t value = 0;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) { 
    deviceConnected = true; 
    Serial.println("*** CONNECTED ***");
  }
  void onDisconnect(BLEServer* pServer) { 
    deviceConnected = false; 
    Serial.println("*** DISCONNECTED ***");
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 BLE...");

  // Use simple name
  BLEDevice::init("ESP32");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Characteristic with NOTIFY only (works better for Web BLE)
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_NOTIFY
  );

  // Add BLE2902 descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  // Advertising - KEEP IT SIMPLE
  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  
  // Web Bluetooth specific settings
  adv->setScanResponse(true);
  adv->setMinPreferred(0x00);     // Web Bluetooth likes this
  adv->setMaxPreferred(0x00);     // Web Bluetooth likes this
  
  // Try with and without this line
  //BLEDevice::setMTU(23);          // Force legacy mode

  adv->start();
  Serial.println("Advertising as 'ESP32-C6'...");
}

void loop() {
  if (deviceConnected) {
    int value = analogRead(3);
    pCharacteristic->setValue(value);
    pCharacteristic->notify();
    
    Serial.print("Sent: ");
    Serial.println(value);
    
    //value++;
    delay(1000);
  }
  if(!deviceConnected && oldDeviceConnected){
    Serial.println("Device Disconnected");
    delay(500);
    pServer->startAdvertising();
    Serial.println("Start Advertising");
    oldDeviceConnected = deviceConnected;
  }
  if(deviceConnected && !oldDeviceConnected){
    oldDeviceConnected = deviceConnected;
  }
}
