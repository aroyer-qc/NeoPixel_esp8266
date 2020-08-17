/*
Lucioles effect for trap in bottles (Harry Potters theme)
Alain Royer, 2020
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#define PIN            13           // Which pin on the ESP8266 is connected to the NeoPixels?
#define NUMPIXELS      3            // How many NeoPixels are attached to the ESP8266?

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "Lucioles Magiques";   
const char* password = "Sonya";
ESP8266WebServer server(80); // set to port 80 as server

int delayval = 16; // delay for half a second

// for test
int LED = 2;


void sentvar()
{
    if (server.hasArg("sensor_reading")) // Value that sent from the client
    {
        int readingInt = server.arg("sensor_reading").toInt();
        char readingToPrint[5];
        itoa(readingInt, readingToPrint, 10); //set the value to string conversion 
    
    //u8g2.firstPage();
    //u8g2.drawUTF8(0, 64, readingToPrint);
    //u8g2.nextPage();
        server.send(200, "text/html", "Value Received");
    }
}

void setup()
{
    Serial.println("Lucioles Magiques");
    Serial.println("Initializing as server");

    WiFi.softAP(ssid, password); //Start Soft AP
    IPAddress myIP = WiFi.softAPIP();

    server.on("/data/", HTTP_GET, sentvar); // Set to the server receives a request with /data/ in the string then run the sentvar function
    server.begin();
  
    pixels.begin(); // This initializes the NeoPixel library.
}

void loop()
{
    // put your main code here, to run repeatedly:
    for(int i = 0; i < NUMPIXELS; i++)
    {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
        
        // TODO remove this and use timing to enter here
        //delay(delayval); // Delay for a period of time (in milliseconds).
    }

    server.handleClient();
}



/*  
#include <ESP8266WiFi.h>
#include <U8g2lib.h>


const char *ssid = "yourssid"; //Server SSI
const char *password = "yourpassword"; //Server Password

ESP8266WebServer server(80); // set to port 80 as server

void sentvar() {
  if (server.hasArg("sensor_reading")) { // Value that sent from the client
    int readingInt = server.arg("sensor_reading").toInt();
    char readingToPrint[5];
    itoa(readingInt, readingToPrint, 10); //set the value to string conversion 
    u8g2.firstPage();
    u8g2.drawUTF8(0, 64, readingToPrint);
    u8g2.nextPage();
    server.send(200, "text/html", "Value Received");
  }
}

void setup() {
  Serial.println("14CORE | MCP9808 - P2P READING TEST")
  delay(1000);
  Serial.println("14CORE | Initializing as server")
  delay(1000);

  u8g2.begin();
  u8g2.setFont(u8g2_font_logisoso62_tn);
  u8g2.setFontMode(0);    //Set to enable transparent mode, which is faster

  WiFi.softAP(ssid, password); //Start Soft AP
  IPAddress myIP = WiFi.softAPIP();

  server.on("/data/", HTTP_GET, sentvar); // Set to the server receives a request with /data/ in the string then run the sentvar function
  server.begin();
}

void loop() {
  server.handleClient();
  }
 */
