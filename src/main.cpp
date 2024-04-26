#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoRS485.h>

// MAC ve IP adresleri
byte mac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
IPAddress ip(192, 168, 0, 200);
EthernetServer server(1234);  // Düz soket programlama için farklı bir port (örneğin 1234)

int ledPin = 5;

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);

    Ethernet.begin(mac, ip);
    server.begin();

    Serial.print("Server IP address: ");
    Serial.println(Ethernet.localIP());
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        Serial.println("New client connected");

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);

                // Gelen veriye göre LED kontrolü
                if (c == '1') {
                    Serial.println("Received ON command");
                    digitalWrite(ledPin, HIGH);
                    client.println("selam");
                } else if (c == '0') {
                    Serial.println("Received OFF command");
                    digitalWrite(ledPin, LOW);
                }

                client.stop();
                Serial.println("Client disconnected");

                
            }
        }
    }
}
