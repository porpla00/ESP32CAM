#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

extern int gpLb = 2;   // Left 1
extern int gpLf = 14;  // Left 2
extern int gpRb = 1;  // Right 1
extern int gpRf = 13;  // Right 2
extern int gpLed = 4;  // Light
extern int Relay = 12; // Relay

#define SERVICE_UUID "f913077c-f401-48d3-ac66-a701317b97b7"
#define CHARACTERISTIC_UUID "0529c11d-1403-4237-aeef-3e062afbccf5"

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value == "0") {

      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, LOW);
      digitalWrite(gpLed, LOW);
      digitalWrite(Relay, LOW);
    } 
    else if (value == "8") {
      // ให้ขา Motor Driver 4 ตัวหมุนจาก high เป็น low
      digitalWrite(gpLb, HIGH);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, HIGH);
      digitalWrite(gpRf, LOW);
      delay(500);

      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, LOW);
    } 
    else if (value == "4") {
      // ให้ขา Motor Driver 4 ตัวหมุนจาก high เป็น low
      digitalWrite(gpLb, HIGH);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, HIGH);
      delay(100);

      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, LOW);
    } 
    else if (value == "5") {
      // ให้ขา Motor Driver 4 ตัวหมุนจาก high เป็น low
      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, LOW);
    } 
    else if (value == "6") {
      // ให้ขา Motor Driver 4 ตัวหมุนจาก high เป็น low
      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, HIGH);
      digitalWrite(gpRb, HIGH);
      digitalWrite(gpRf, LOW);
      delay(100);

      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, LOW);
    } 
    else if (value == "2") {
      // ให้ขา Motor Driver 4 ตัวหมุนจาก high เป็น low
      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, HIGH);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, HIGH);
      digitalWrite(Relay, HIGH);
      delay(500);

      digitalWrite(gpLb, LOW);
      digitalWrite(gpLf, LOW);
      digitalWrite(gpRb, LOW);
      digitalWrite(gpRf, LOW);
      digitalWrite(Relay, LOW);
    } 
    else if (value == "7") {
      digitalWrite(gpLed, HIGH);
    } 
    else if (value == "9") {
      digitalWrite(gpLed, LOW);
    }
  }
};


void setup() {
  Serial.begin(115200);
  pinMode(gpLb, OUTPUT);   //Left Backward
  pinMode(gpLf, OUTPUT);   //Left Forward
  pinMode(gpRb, OUTPUT);   //Right Forward
  pinMode(gpRf, OUTPUT);   //Right Backward
  pinMode(gpLed, OUTPUT);  //Light
  pinMode(Relay, OUTPUT);  // Relay

  BLEDevice::init("Thunkable & esp32");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Turn me on/off");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
}