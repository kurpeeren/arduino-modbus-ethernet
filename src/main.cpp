#include <EtherCard.h> //EtherCard kütüphanesi ekleme
#include <SPI.h>
#include <ArduinoRS485.h>

//Ledi yakıp söndürmek için kısayollar
//http://192.168.2.34/?LED=OFF
//http://192.168.2.34/?LED=ON


#define STATIC 0 // DHCP'yi kullanım dışı bırakmak için 1 yapılır (myip/gwip değerlerini aşağıda ayarlanır)

void blinkLed();

// mac adresi
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet ip adresi
static byte myip[] = { 192,168,0,200 };
// gateway ip adresi
static byte gwip[] = { 192,168,0,1 };

// Kontrol edilecek LED pini
int ledPin = 5; //Arduino D5 pini ile GND arasına LED ve 150ohm direnç bağlanır.

byte Ethernet::buffer[700];

//web arayüzü
char const page[] PROGMEM =
"HTTP/1.0 503 Service Unavailable\r\n"
"Content-Type: text/html\r\n"
"Retry-After: 600\r\n"
"\r\n"
"<html>"
"<head><title>"
"Service Temporarily Unavailable"
"</title></head>"
"<body>"
"<h3>LED KONTROL PROGRAMI</h3>"
"<p><em>"
"LED kontrolu icin Arduino'ya iletilecek komutlar.<br />"
"Sintaks yapisi: http://192.168.0.XX/?LED=OFF or ON"
"</em></p>"
"</body>"
"</html>"
;

void blinkLed()
{
    while (true)
    {
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(500);
    }
}
void setup () 
{
    pinMode(ledPin, OUTPUT);
    
    Serial.begin(9600);
    Serial.println("Trying to get an IP...");
    
    Serial.print("MAC: ");
    for (byte i = 0; i < 6; ++i) 
    {
        Serial.print(mymac[i], HEX);
        if (i < 5)
        Serial.print(':');
    }
    Serial.println();
    
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    {
        Serial.println( "Failed to access Ethernet controller");
    }
    else
    {
        Serial.println("Ethernet controller access: OK");
    }
;

#if STATIC
    Serial.println( "Getting static IP.");
    if (!ether.staticSetup(myip, gwip))
    {
      Serial.println( "could not get a static IP");
      blinkLed(); // blink forever to indicate a problem
    }
#else

    Serial.println("Setting up DHCP");
    if (!ether.dhcpSetup())
    {
        Serial.println( "DHCP failed");
        blinkLed(); // blink forever to indicate a problem
    }
#endif

    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.netmask);
    ether.printIp("GW IP: ", ether.gwip);
    ether.printIp("DNS IP: ", ether.dnsip);
}

void loop () 
{
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);

    // IF LED=ON turn it ON
    if(strstr((char *)Ethernet::buffer + pos, "GET /?LED=ON") != 0) 
    {
        Serial.println("Received ON command");
        digitalWrite(ledPin, HIGH);
    }
    
    // IF LED=OFF turn it OFF
    if(strstr((char *)Ethernet::buffer + pos, "GET /?LED=OFF") != 0) 
    {
        Serial.println("Received OFF command");
        digitalWrite(ledPin, LOW);
    }
    
    // show some data to the user
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
}




