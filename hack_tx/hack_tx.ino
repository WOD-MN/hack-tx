#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 5, TXPin = 4;
static const uint32_t GPSBaud = 9600;
#define LORA_SS 15   // LoRa radio chip select (CS) pin
#define LORA_RST 16 // LoRa radio reset (RST) pin
#define LORA_DI0 4  // LoRa radio digital input (DIO0) pin


// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  ss.begin(GPSBaud);
  LoRa.begin(433E6);
  LoRa.setTxPower(18);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(10);
  LoRa.setSyncWord(0xF3);
  LoRa.enableCrc();
}

void loop() {
  smartDelay(5000);

  // If GPS data is available, print latitude and longitude
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", Longitude: ");
    Serial.println(gps.location.lng(), 6);

    // Send latitude and longitude via LoRa
    LoRa.beginPacket();
    LoRa.print(gps.location.lat(), 6);
    LoRa.print(",");
    LoRa.println(gps.location.lng(), 6);
    LoRa.endPacket();
  }
}

static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
