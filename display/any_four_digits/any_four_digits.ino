#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#include <Adafruit_LEDBackpack.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ctype.h>

const char* ssid = "nummer 19B";
const char* password = "gezelligThuis";

WiFiUDP UDP;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "Hi there! Got the message :-)";  // a reply string to send back

Adafruit_7segment matrix = Adafruit_7segment();


void setup()
{
  // Setup LED display
  Wire.begin(4, 5);
  matrix.begin(0x70);
  matrix.clear();
  matrix.writeDisplay();
  matrix.setBrightness(1);
  
  // Initialize serial
  Serial.begin(115200);

  // i can haz internetz
  connectWifiAndSetupUDP();
}

void loop()
{
  delay(1000);
  int packetSize = UDP.parsePacket();
  // packetSize is '0' when there is no packet and '0' is false in C 
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, UDP.remoteIP().toString().c_str(), UDP.remotePort());
    if (packetSize > 5)
    {
      printError(String("Packet is too big: ") + packetSize);
      return;
    }
    int len = UDP.read(incomingPacket, 255);

    // Set a terminating NULL character for atof
    if (len > 0)
    {
      incomingPacket[len] = '\0';
    }
    Serial.printf("UDP packet contents: %s", incomingPacket);
    if (isPositiveNaturalNumber(incomingPacket, packetSize))
    {
      Serial.print("Printing: ");
      Serial.println(atoi(incomingPacket));
      matrix.print(atoi(incomingPacket));
      matrix.writeDisplay();
    }
    else 
    {
      printError(String("This is not a float: ") + incomingPacket + "\n");
    }
    // send back a reply, to the IP address and port we got the packet from
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write(replyPacekt);
    UDP.endPacket();      
  }
}

void printError(String message)
{
  Serial.print(message);
  matrix.printError();
  matrix.writeDisplay();  
}

bool isPositiveNaturalNumber(char* input, int sizeOfInput) 
{
  for(int i = 0; i < sizeOfInput - 1; i++)
  {
    if(!isdigit(input[i]))
    {
      return false;
    }
  }
  return true;
}

void connectWifiAndSetupUDP() 
{
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  UDP.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}
