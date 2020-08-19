/*
Lucioles effect for trap in bottles (Harry Potters theme)
Alain Royer, 2020
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

#define PIN            13           // Which pin on the ESP8266 is connected to the NeoPixels?
#define NUMPIXELS      3            // How many NeoPixels are attached to the ESP8266?

const char* ssid = "Lucioles Magiques";   
const char* password = "Sonya";

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80); // set to port 80 as server


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

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
        server.send(200, "text/html", "Value Received");
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Lucioles Magiques");
    Serial.println("Initializing as server");

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP());

    server.on("/",        handle_OnConnect);
    server.on("/led1on",  handle_led1on);
    server.on("/led1off", handle_led1off);
    server.on("/led2on",  handle_led2on);
    server.on("/led2off", handle_led2off);
    server.onNotFound(handle_NotFound);

    server.on("/", HTTP_GET, sentvar); // Set to the server receives a request with /data/ in the string then run the sentvar function
    server.begin();
    Serial.println("HTTP server started");
  
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


void handle_OnConnect()
{
    //LED1status = LOW;
    //LED2status = LOW;
    Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
    server.send(200, "text/html", SendHTML(false, false /*LED1status,LED2status*/)); 
}

void handle_led1on()
{
    //LED1status = HIGH;
    Serial.println("GPIO7 Status: ON");
    server.send(200, "text/html", SendHTML(true, true/*LED2status*/)); 
}

void handle_led1off()
{
    //LED1status = LOW;
    Serial.println("GPIO7 Status: OFF");
    server.send(200, "text/html", SendHTML(false, true /*LED2status*/)); 
}

void handle_led2on()
{
    //LED2status = HIGH;
    Serial.println("GPIO6 Status: ON");
    server.send(200, "text/html", SendHTML(false/*LED1status*/, true)); 
}

void handle_led2off()
{
    //LED2status = LOW;
    Serial.println("GPIO6 Status: OFF");
    server.send(200, "text/html", SendHTML(true/*LED1status*/, false)); 
}

void handle_NotFound()
{
    server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat)
{
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr +="<title>LED Control</title>\n";
    ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
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
    ptr +="<h3>Professeure Sylvain</h3>\n";
  
    if(led1stat)
        {ptr +="<p>Lucioles 1-2-3: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
    else
        {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

    if(led2stat)
        {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
    else
        {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

    ptr +="</body>\n";
    ptr +="</html>\n";
    return ptr;
}
