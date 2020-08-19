/*
Lucioles effect for trap in bottles (Harry Potters theme)
Alain Royer, 2020
*/
#define USE_ESP32

#ifdef USE_ESP32
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#else
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif

#include <Adafruit_NeoPixel.h>

#define PIN            13           // Which pin on the ESP8266 is connected to the NeoPixels?
#define NUMPIXELS      3            // How many NeoPixels are attached to the ESP8266?

#define LUCIOLE_1      1
#define LUCIOLE_2      2
#define LUCIOLE_3      4

const char* ssid = "Lucioles Magiques";   
const char* password = "Magic";

// Put IP Address details
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

#ifdef USE_ESP32
AsyncWebServer server(80);          // Set to port 80 as server
#else
ESP8266WebServer server(80);        // Set to port 80 as server
#endif

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 16; // delay for half a second

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Lucioles Magiques");
    Serial.println("Initialiser serveur");

    #ifdef USE_ESP32
    WiFi.mode(WIFI_AP);
    #endif
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(1000);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP Address IP: ");
    Serial.println(IP);

    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP());
   
#ifdef USE_ESP32
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
        request->send(200, "/text.html",  SendHTML(0));
    });

    server.on("/All", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        Serial.println("Toutes les Lucioles sweep");
        request->send(200, "/text.html",  SendHTML(LUCIOLE_1 | LUCIOLE_2 | LUCIOLE_3));
    });

    server.on("/luc1", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        Serial.println("Luciole 1 sweep");
        request->send(200, "/text.html",  SendHTML(LUCIOLE_1));
    });

    server.on("/luc2", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        Serial.println("Luciole 2 sweep");
        request->send(200, "/text.html",  SendHTML(LUCIOLE_2));
    });

    server.on("/luc3", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        Serial.println("Luciole 3 sweep");
        request->send(200, "/text.html",  SendHTML(LUCIOLE_3));
    });
#else
    server.on("/",          HandleOnConnect);
    server.on("/All",       HandleAll);
    server.on("/luc1",      HandleLuciole_1);
    server.on("/luc2",      HandleLuciole_2);
    server.on("/luc3",      HandleLuciole_3);
    server.onNotFound(HandleNotFound);
#endif

    server.begin();
    Serial.println("Serveur HTTP Prêt");
  
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

    #ifndef USE_ESP32
    server.handleClient();
    #endif
}

#ifndef USE_ESP32
void HandleOnConnect()
{
    Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
    server.send(200, "text/html", SendHTML(0)); 
}

void HandleAll()
{
    Serial.println("Toutes les Lucioles sweep");
    server.send(200, "text/html", SendHTML(LUCIOLE_1 | LUCIOLE_2 | LUCIOLE_3)); 
}

void HandleLuciole_1()
{
    Serial.println("Luciole 1 sweep");
    server.send(200, "text/html", SendHTML(LUCIOLE_1)); 
}

void HandleLuciole_2()
{
    Serial.println("Luciole 2 sweep");
    server.send(200, "text/html", SendHTML(LUCIOLE_2)); 
}

void HandleLuciole_3()
{
    Serial.println("Luciole 3 sweep");
    server.send(200, "text/html", SendHTML(LUCIOLE_3)); 
}

void HandleNotFound()
{
    server.send(404, "text/plain", "404, Non Trouvé");
}
#endif

String SendHTML(uint8_t Lucioles)
{
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr +="<title>Contrôle des lucioles</title>\n";
    ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    
    
    ptr +="<div class=\"container\">";
    ptr +="<div class=\"led-box\"><div class=\"led-green\"></div><p>Green LED</p></div>";
    ptr +="<div class=\"led-box\"><div class=\"led-yellow\"></div><p>Yellow LED</p></div>";
    ptr +="<div class=\"led-box\"><div class=\"led-red\"></div><p>Red LED</p></div>";
    ptr +="<div class=\"led-box\"><div class=\"led-blue\"></div><p>Blue LED</p></div>";
    ptr +="</div>";    
    
    // CSS
    ptr +=".container {";
    ptr +="background-size: cover;";
    ptr +="background: rgb(226,226,226);"; /* Old browsers */
    ptr +="background: -moz-linear-gradient(top,  rgba(226,226,226,1) 0%, rgba(219,219,219,1) 50%, rgba(209,209,209,1) 51%, rgba(254,254,254,1) 100%);"; /* FF3.6+ */
    ptr +="       background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,rgba(226,226,226,1)), color-stop(50%,rgba(219,219,219,1)), color-stop(51%,rgba(209,209,209,1)), color-stop(100%,rgba(254,254,254,1)));"; /* Chrome,Safari4+ */
    ptr +="       background: -webkit-linear-gradient(top,  rgba(226,226,226,1) 0%,rgba(219,219,219,1) 50%,rgba(209,209,209,1) 51%,rgba(254,254,254,1) 100%);"; /* Chrome10+,Safari5.1+ */
    ptr +="       background: -o-linear-gradient(top,  rgba(226,226,226,1) 0%,rgba(219,219,219,1) 50%,rgba(209,209,209,1) 51%,rgba(254,254,254,1) 100%);"; /* Opera 11.10+ */
    ptr +="       background: -ms-linear-gradient(top,  rgba(226,226,226,1) 0%,rgba(219,219,219,1) 50%,rgba(209,209,209,1) 51%,rgba(254,254,254,1) 100%);"; /* IE10+ */
    ptr +="       background: linear-gradient(to bottom,  rgba(226,226,226,1) 0%,rgba(219,219,219,1) 50%,rgba(209,209,209,1) 51%,rgba(254,254,254,1) 100%);"; /* W3C */
    ptr +="filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#e2e2e2', endColorstr='#fefefe',GradientType=0 );"; /* IE6-9 */
    ptr +="padding: 20px;";
    ptr +="}";

    ptr +=".led-box {";
    ptr +="height: 30px;";
    ptr +="width: 25%;";
    ptr +="margin: 10px 0;";
    ptr +="float: left;";
    ptr +="}";

    ptr +=".led-box p {";
    ptr +="font-size: 12px;";
    ptr +="text-align: center;";
    ptr +="margin: 1em;";
    ptr +="}";

    ptr +=".led-red {";
    ptr +="margin: 0 auto;";
    ptr +="width: 24px;";
    ptr +="height: 24px;";
    ptr +="background-color: #F00;";
    ptr +="border-radius: 50%;";
    ptr +="box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #441313 0 -1px 9px, rgba(255, 0, 0, 0.5) 0 2px 12px;";
    ptr +="-webkit-animation: blinkRed 0.5s infinite;";
    ptr +="-moz-animation: blinkRed 0.5s infinite;";
    ptr +="-ms-animation: blinkRed 0.5s infinite;";
    ptr +="-o-animation: blinkRed 0.5s infinite;";
    ptr +="animation: blinkRed 0.5s infinite;";
    ptr +="}";

    ptr +="@-webkit-keyframes blinkRed {";
    ptr +="from { background-color: #F00; }";
    ptr +="50% { background-color: #A00; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #441313 0 -1px 9px, rgba(255, 0, 0, 0.5) 0 2px 0;}";
    ptr +="to { background-color: #F00; }";
    ptr +="}";
    ptr +="@-moz-keyframes blinkRed {";
    ptr +="from { background-color: #F00; }";
    ptr +="50% { background-color: #A00; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #441313 0 -1px 9px, rgba(255, 0, 0, 0.5) 0 2px 0;}";
    ptr +="to { background-color: #F00; }";
    ptr +="}";
    ptr +="@-ms-keyframes blinkRed {";
    ptr +="from { background-color: #F00; }";
    ptr +="50% { background-color: #A00; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #441313 0 -1px 9px, rgba(255, 0, 0, 0.5) 0 2px 0;}";
    ptr +="to { background-color: #F00; }";
    ptr +="}";
    ptr +="@-o-keyframes blinkRed {";
    ptr +="from { background-color: #F00; }";
    ptr +="50% { background-color: #A00; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #441313 0 -1px 9px, rgba(255, 0, 0, 0.5) 0 2px 0;}";
    ptr +="to { background-color: #F00; }";
    ptr +="}";
    ptr +="@keyframes blinkRed {";
    ptr +="from { background-color: #F00; }";
    ptr +="50% { background-color: #A00; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #441313 0 -1px 9px, rgba(255, 0, 0, 0.5) 0 2px 0;}";
    ptr +="to { background-color: #F00; }";
    ptr +="}";

    ptr +=".led-yellow {";
    ptr +="margin: 0 auto;";
    ptr +="width: 24px;";
    ptr +="height: 24px;";
    ptr +="background-color: #FF0;";
    ptr +="border-radius: 50%;";
    ptr +="box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #808002 0 -1px 9px, #FF0 0 2px 12px;";
    ptr +="-webkit-animation: blinkYellow 1s infinite;";
    ptr +="-moz-animation: blinkYellow 1s infinite;";
    ptr +="-ms-animation: blinkYellow 1s infinite;";
    ptr +="-o-animation: blinkYellow 1s infinite;";
    ptr +="animation: blinkYellow 1s infinite;";
    ptr +="}";

    ptr +="@-webkit-keyframes blinkYellow {";
    ptr +="from { background-color: #FF0; }";
    ptr +="50% { background-color: #AA0; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #808002 0 -1px 9px, #FF0 0 2px 0; }";
    ptr +="to { background-color: #FF0; }";
    ptr +="}";
    ptr +="@-moz-keyframes blinkYellow {";
    ptr +="from { background-color: #FF0; }";
    ptr +="50% { background-color: #AA0; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #808002 0 -1px 9px, #FF0 0 2px 0; }";
    ptr +="to { background-color: #FF0; }";
    ptr +="}";
    ptr +="@-ms-keyframes blinkYellow {";
    ptr +="from { background-color: #FF0; }";
    ptr +="50% { background-color: #AA0; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #808002 0 -1px 9px, #FF0 0 2px 0; }";
    ptr +="to { background-color: #FF0; }";
    ptr +="}";
    ptr +="@-o-keyframes blinkYellow {";
    ptr +="from { background-color: #FF0; }";
    ptr +="50% { background-color: #AA0; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #808002 0 -1px 9px, #FF0 0 2px 0; }";
    ptr +="to { background-color: #FF0; }";
    ptr +="}";
    ptr +="@keyframes blinkYellow {";
    ptr +="from { background-color: #FF0; }";
    ptr +="50% { background-color: #AA0; box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #808002 0 -1px 9px, #FF0 0 2px 0; }";
    ptr +="to { background-color: #FF0; }";
    ptr +="}";

    ptr +=".led-green {";
    ptr +="margin: 0 auto;";
    ptr +="width: 24px;";
    ptr +="height: 24px;";
    ptr +="background-color: #ABFF00;";
    ptr +="border-radius: 50%;";
    ptr +="box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #304701 0 -1px 9px, #89FF00 0 2px 12px;";
    ptr +="}";

    ptr +=".led-blue {";
    ptr +="margin: 0 auto;";
    ptr +="width: 24px;";
    ptr +="height: 24px;";
    ptr +="background-color: #24E0FF;";
    ptr +="border-radius: 50%;";
    ptr +="box-shadow: rgba(0, 0, 0, 0.2) 0 -1px 7px 1px, inset #006 0 -1px 9px, #3F8CFF 0 2px 14px;";
    ptr +="}";
    
    // JS
    ptr +="$( function() {";
    ptr +="var $winHeight = $( window ).height()";
    ptr +="$( '.container' ).height( $winHeight );";
    ptr +="});";

    // Button
    ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
    ptr +=".button-on {background-color: #1abc9c;}\n";
    ptr +=".button-on:active {background-color: #16a085;}\n";
    ptr +=".button-off {background-color: #34495e;}\n";
    ptr +=".button-off:active {background-color: #2c3e50;}\n";
    ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
    ptr +="</style>\n";
    ptr +="</head>\n";
    ptr +="<body>\n";
    ptr +="<h1>Lucioles Magiques</h1>\n";
    ptr +="<h3>Copyright (c) Alain Royer 2020</h3>\n";
  
    if(Lucioles & LUCIOLE_1) { ptr +="<p>Lucioles 1: ON  </p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n"; }
    else                     { ptr +="<p>Lucioles 1: OFF </p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";    }
    if(Lucioles & LUCIOLE_2) { ptr +="<p>Lucioles 2: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";   }
    else                     { ptr +="<p>Lucioles 2: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";     }
    if(Lucioles & LUCIOLE_3) { ptr +="<p>Lucioles 3: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";   }
    else                     { ptr +="<p>Lucioles 3: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";     }

    ptr +="</body>\n";
    ptr +="</html>\n";
    return ptr;
}
