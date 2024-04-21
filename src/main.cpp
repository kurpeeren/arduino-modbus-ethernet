#include <Arduino.h>
#include <ArduinoRS485.h>
#include <SoftwareSerial.h>
#include <ArduinoModbus.h>
#include <Ethernet.h>



// Ethernet ayarları
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Ethernet kartınızın MAC adresi
IPAddress ip(192, 168, 1, 100); // Arduino'nun IP adresi
IPAddress gateway(192, 168, 1, 1); // Ağ geçidi
IPAddress subnet(255, 255, 255, 0); // Alt ağ maskesi

// RS485 ayarları
#define RS485TxEnablePin 2 // RS485 için TxEnable pini
#define RS485Serial Serial1 // RS485 için kullanılacak seri port

// Modbus ayarları
ModbusIP mb;
const uint16_t regCount = 10; // Okunacak register sayısı
uint16_t holdingRegs[regCount]; // Okunan register değerlerini saklamak için dizi

void setup() {
  Serial.begin(9600);
  RS485Serial.begin(9600);
  
  // Ethernet başlatma
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  mb.server(); // Modbus sunucu olarak başlatma

  // RS485 ayarları
  pinMode(RS485TxEnablePin, OUTPUT);
  digitalWrite(RS485TxEnablePin, LOW); // RS485 modunda dinleme moduna geçme

  Serial.println("Setup completed.");
}

void loop() {
  mb.task();

  // RS485 üzerinden veri okuma
  digitalWrite(RS485TxEnablePin, HIGH); // RS485 modunda veri okuma
  if (RS485Serial.available() >= 4) {
    uint8_t buffer[4];
    RS485Serial.readBytes(buffer, 4);
    holdingRegs[0] = word(buffer[0], buffer[1]); // Örnek olarak ilk 2 baytı bir register'a atama
    holdingRegs[1] = word(buffer[2], buffer[3]); // Sonraki 2 baytı başka bir register'a atama
  }
  digitalWrite(RS485TxEnablePin, LOW); // RS485 modunda dinleme moduna geçme

  // Modbus üzerinden okunan verileri yayınlama
  mb.Hreg(0, holdingRegs[0]);
  mb.Hreg(1, holdingRegs[1]);
}