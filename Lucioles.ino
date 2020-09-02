
/*
Lucioles effect for trap in bottles (Harry Potters theme)
Alain Royer, 2020
*/
#define USE_ESP32
#define USE_WIFI

// ****************************************************************************
// Includes 

#ifdef USE_WIFI
 #ifdef USE_ESP32
  #include <WiFi.h>
  #include "ESPAsyncWebServer.h"
 #else
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
 #endif // USE_ESP32
#endif // USE_WIFI

//These define's must be placed at the beginning before #include "ESP x TimerInterrupt.h"
//#define TIMER_INTERRUPT_DEBUG      1

#ifdef USE_ESP32
 #include <ESP32TimerInterrupt.h>
 //#include <ESP32_ISR_Timer-Impl.h>
 //#include <ESP32_ISR_Timer.h>
#else
#include <ESP8266TimerInterrupt.h>
//#include <ESP8266_ISR_Timer-Impl.h>
//#include <ESP8266_ISR_Timer.h>
#endif
 
#include <Adafruit_NeoPixel.h>

// ****************************************************************************
// Defines 

#define PIN                     13
#define PIN_BUTTON              0

#define TIMER_INTERVAL_MS       1000

#define MAX_STEP                26
#define MIN_RANDOM_INTENSITY    7
#define MAX_RANDOM_INTENSITY    MAX_STEP
#define MIN_RANDOM_TIMING       5000                 // 5 Seconds
#define MAX_RANDOM_TIMING       20000                // 20 Seconds

#define RELEASED                0
#define PRESSED                 1
#define LONG_PRESSED            2

#define DelayHasEnded(DELAY)  ((millis() > DELAY) ? true : false)

// ****************************************************************************
// Typedefs 

typedef enum
{
    PIXEL_0,
    PIXEL_1,
    PIXEL_2,
    PIXEL_3,
    PIXEL_4,
    PIXEL_5,
    PIXEL_6,
    PIXEL_7,
    
    NUMBER_OF_PIXEL,
    FIRST_PIXEL = 0,
} Pixel_t;

// ****************************************************************************
// Variables 

#ifdef USE_WIFI
 const char* ssid = "Lucioles Magiques";   
 //const char* password = "Magic";

 // Put IP Address details
 IPAddress local_ip(192,168,1,1);
 IPAddress gateway(192,168,1,1);
 IPAddress subnet(255,255,255,0);

 #ifdef USE_ESP32
  AsyncWebServer server(80);          // Set to port 80 as server
 #else
  ESP8266WebServer server(80);        // Set to port 80 as server
 #endif // USE_ESP32

#endif // USE_WIFI

uint32_t TickCounter;

 #ifdef USE_ESP32
  ESP32Timer Timer(0);
 #else
  ESP8266Timer Timer(1);
 #endif // USE_ESP32

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMBER_OF_PIXEL, PIN, NEO_GRB + NEO_KHZ800);

const uint8_t PixPercent[MAX_STEP] =
{
    0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 100,   // Ascending 
    64, 48, 32, 24, 16, 12, 8, 6, 4, 2, 1, 0};          // Descending

bool IsIsTimeToUpdate = false;
bool RequestAllPixel  = false;

uint8_t  PixRed[NUMBER_OF_PIXEL];
uint8_t  PixGreen[NUMBER_OF_PIXEL];
uint8_t  PixBlue[NUMBER_OF_PIXEL];
uint32_t PixTiming[NUMBER_OF_PIXEL];
uint32_t PixStep[NUMBER_OF_PIXEL];

uint16_t  MinRandomPixelTiming    = MIN_RANDOM_TIMING;
uint16_t  MaxRandomPixelTiming    = MAX_RANDOM_TIMING;
uint16_t  MinRandomPixelIntensity = MIN_RANDOM_INTENSITY;
uint16_t  MaxRandomPixelIntensity = MAX_RANDOM_INTENSITY;


int      ButtonState      = RELEASED;
bool     ButtonStateCount = 0;
int      ButtonAction     = PRESSED;

String CSS_Button = "\
input[type=button] {\
  background-color: #3071a9;\
  border: 0.2px solid #010101;\
  border-radius: 4px;\
  color: white;\
  padding: 8px 16px;\
  text-decoration: none;\
  margin: 4px 2px;\
  cursor: pointer;\
}";


String CSS_Range = "\
input[type=range] {\
  width: 100%;\
  margin: 13.8px 0;\
  background-color: transparent;\
  -webkit-appearance: none;\
}\
input[type=range]:focus {\
outline: none;\
}\
input[type=range]::-webkit-slider-runnable-track {\
  background: #3071a9;\
  border: 0.2px solid #010101;\
  border-radius: 1.3px;\
  width: 100%;\
  height: 8.4px;\
  cursor: pointer;\
}\
input[type=range]::-webkit-slider-thumb {\
  margin-top: -14px;\
  width: 16px;\
  height: 36px;\
  background: #ffffff;\
  border: 1px solid #000000;\
  border-radius: 3px;\
  cursor: pointer;\
  -webkit-appearance: none;\
}\
input[type=range]:focus::-webkit-slider-runnable-track {\
  background: #367ebd;\
}\
input[type=range]::-moz-range-track {\
  background: #3071a9;\
  border: 0.2px solid #010101;\
  border-radius: 1.3px;\
  width: 100%;\
  height: 8.4px;\
  cursor: pointer;\
}\
input[type=range]::-moz-range-thumb {\
  width: 16px;\
  height: 36px;\
  background: #ffffff;\
  border: 1px solid #000000;\
  border-radius: 3px;\
  cursor: pointer;\
}\
input[type=range]::-ms-track {\
  background: transparent;\
  border-color: transparent;\
  border-width: 14.8px 0;\
  color: transparent;\
  width: 100%;\
  height: 8.4px;\
  cursor: pointer;\
}\
input[type=range]::-ms-fill-lower {\
  background: #2a6495;\
  border: 0.2px solid #010101;\
  border-radius: 2.6px;\
}\
input[type=range]::-ms-fill-upper {\
  background: #3071a9;\
  border: 0.2px solid #010101;\
  border-radius: 2.6px;\
}\
input[type=range]::-ms-thumb {\
  width: 16px;\
  height: 36px;\
  background: #ffffff;\
  border: 1px solid #000000;\
  border-radius: 3px;\
  cursor: pointer;\
  margin-top: 0px;\
}\
input[type=range]:focus::-ms-fill-lower {\
  background: #3071a9;\
}\
input[type=range]:focus::-ms-fill-upper {\
  background: #367ebd;\
}\
/@supports (-ms-ime-align:auto) {\
  input[type=range] {\
    margin: 0;\
  }\
}";

String CSS_Submit = "\
input[type=submit] {\
  background-color: #3071a9;\
  border: 0.2px solid #010101;\
  border-radius: 4px;\
  color: white;\
  padding: 8px 16px;\
  text-decoration: none;\
  margin: 4px 2px;\
  cursor: pointer;\
}";


// ****************************************************************************

void IRAM_ATTR TimerHandler(void)
{
    TickCounter++;

    if(ButtonState == PRESSED)
    {
        Serial.printf(".");

        if(ButtonStateCount < 1000)
        {
          ButtonStateCount++;
        }
        else
        {
            ButtonAction = LONG_PRESSED;
        }
    }
    else
    {
        if((ButtonStateCount >= 10) &&
           (ButtonStateCount < 200) &&
           (ButtonAction == RELEASED))
        {
            ButtonAction = PRESSED;
        }
        ButtonStateCount = 0;
        
        if(ButtonAction == LONG_PRESSED)
        {
            ButtonAction = RELEASED;
        }
    }

    #if 0
  #ifdef USE_ESP32
    static bool Toggle = false;
  
    if((TickCounter % 600) == 0)
    {
        Toggle = !Toggle;
        RequestAllPixel = true;
    }
  #endif  
#endif
}

// ****************************************************************************

void setup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Lucioles Magiques");
    Serial.println("Initialiser serveur");

  #ifdef USE_WIFI
    SetupWifi();
  #endif // USE_WIFI

    pinMode(PIN_BUTTON, INPUT_PULLUP);

    pixels.begin(); // This initializes the NeoPixel library.

    // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    randomSeed(analogRead(0));
  
    // Randomize all value for pixel
    for(int Pixel = (int)FIRST_PIXEL; Pixel < (int)NUMBER_OF_PIXEL; Pixel++)
    {
        SetNextPixel(Pixel);
    }

    // Interval in microsecs
    if(Timer.attachInterruptInterval(TIMER_INTERVAL_MS * 16, TimerHandler))
    {
        Serial.println("Starting  Timer OK, millis() = " + String(millis()));
    }
    else
    {
       Serial.println("Can't set Timer correctly. Select another freq. or interval");
    }
}

// ****************************************************************************

void loop()
{
    static int lastButtonState = RELEASED;
    
    ButtonState = digitalRead(PIN_BUTTON);
    
    
    if(ButtonAction == PRESSED)
    {
       Serial.println("Pressed!"); 
       ButtonAction = RELEASED;
       lastButtonState = PRESSED; 
    }

    if(ButtonAction == LONG_PRESSED)
    {
       Serial.println("Long Pressed!"); 
       lastButtonState = LONG_PRESSED; 
    }

    if(lastButtonState != RELEASED)
    {
        lastButtonState = RELEASED;
        Serial.println("Released!");
    }
    
    if(RequestAllPixel == true)
    {
        for(int Pixel = (int)FIRST_PIXEL; Pixel < (int)NUMBER_OF_PIXEL; Pixel++)
        {
            PixStep[Pixel] = 1;
            IsIsTimeToUpdate = true;
        }
        
        RequestAllPixel = false;
    }
    else
    {
        for(int Pixel = (int)FIRST_PIXEL; Pixel < (int)NUMBER_OF_PIXEL; Pixel++)
        {
            if(PixStep[Pixel] != 0)
            {
               PixStep[Pixel]++;
    
               if(PixStep[Pixel] == MAX_STEP)
               {
                  PixStep[Pixel] = 0;
                  SetNextPixel(Pixel);
               }
               else
               {
                  IsIsTimeToUpdate = true;
                  //Serial.printf("%d - %d \r\n", Pixel, PixPercent[PixStep[Pixel]]);
               }
            }
            else
            {      
                if(DelayHasEnded(PixTiming[Pixel]))
                {
                    PixStep[Pixel]++;
                    IsIsTimeToUpdate = true;
                   //Serial.printf("%d - %d \r\n", Pixel, PixPercent[PixStep[Pixel]]);
                }
            }
        }
    }
  
    if(IsIsTimeToUpdate == true)
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;

        IsIsTimeToUpdate = false;
        
        // Update Pixel
        for(int Pixel = (int)FIRST_PIXEL; Pixel < (int)NUMBER_OF_PIXEL; Pixel++)
        {

            Red   = (uint8_t)(((uint32_t)PixPercent[PixStep[Pixel]] * PixRed  [Pixel]) / 100);
            Green = (uint8_t)(((uint32_t)PixPercent[PixStep[Pixel]] * PixGreen[Pixel]) / 100);
            Blue  = (uint8_t)(((uint32_t)PixPercent[PixStep[Pixel]] * PixBlue [Pixel]) / 100);

            // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
            pixels.setPixelColor(Pixel, pixels.Color(Red, Green, Blue));
            pixels.show(); // This sends the updated pixel color to the hardware.
        }
    }

    delay(4);
    //Serial.println("Timer millis() = " + String(millis()));
    //Serial.printf("%ld", TickCounter);

  #ifdef USE_WIFI
   #ifndef USE_ESP32
    server.handleClient();
   #endif // USE_ESP32
  #endif // USE_WIFI
}

// ****************************************************************************

void SetNextPixel(int PixelID)
{
    // randomize all value for pixel driving
    PixRed  [PixelID]  = random(MIN_RANDOM_INTENSITY, MAX_RANDOM_INTENSITY);
    PixGreen[PixelID]  = random(MIN_RANDOM_INTENSITY, MAX_RANDOM_INTENSITY);
    PixBlue [PixelID]  = random(MIN_RANDOM_INTENSITY, MAX_RANDOM_INTENSITY);
    PixTiming[PixelID] = millis() + random(MIN_RANDOM_TIMING, MAX_RANDOM_TIMING);
}

// ****************************************************************************

#ifdef USE_WIFI

// ****************************************************************************

void SetupWifi()
{
  #ifdef USE_ESP32
    WiFi.mode(WIFI_AP);
  #endif // USE_ESP32
  delay(100);
    WiFi.softAP(ssid/*, password*/);
  delay(100);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(1000);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP Address IP: ");
    Serial.println(IP);

  delay(100);

    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP());

  delay(100);

    Serial.println(WiFi.SSID());

   
  #ifdef USE_ESP32
    server.on("/",          HTTP_GET, [](AsyncWebServerRequest *request) { Serial.println("Main page!");      request->send(200, "text/html",  SendHTML(0)); });
    server.on("/Party",     HTTP_GET, [](AsyncWebServerRequest *request) { Serial.println("Partya Luciola");  request->send(200, "text/html",  SendHTML(1)); });
    server.on("/PartyDone", HTTP_GET, [](AsyncWebServerRequest *request) { Serial.println("Partya is done");  request->send(200, "text/html",  SendHTML(0)); });
    server.on("/MoreParty", HTTP_GET, [](AsyncWebServerRequest *request) { Serial.println("More Partya");     request->send(200, "text/html",  SendHTML(2)); });
    server.on("/LessParty", HTTP_GET, [](AsyncWebServerRequest *request) { Serial.println("Less Partya");     request->send(200, "text/html",  SendHTML(0)); });

    // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request)
    {
        String InputMessage;
        String InputParam;
    
        // GET Min value on <ESP_IP>/get?Min=<inputMessage>
        if(request->hasParam("Min"))
        {
            InputMessage = request->getParam("Min")->value();
            MinRandomPixelTiming = InputMessage.toInt();
            InputParam   = "Min";
            Serial.println("Min: " + InputMessage);
        }
        // GET Max value on <ESP_IP>/get?Max=<inputMessage>
        else if(request->hasParam("Max"))
        {
            InputMessage = request->getParam("Max")->value();
            MaxRandomPixelTiming = InputMessage.toInt();
            InputParam   = "Max";
            Serial.println("Max: " + InputMessage);
        }
        // GET MinBright value on <ESP_IP>/get?MinBright=<inputMessage>
        else if(request->hasParam("IntMin"))
        {
            InputMessage = request->getParam("IntMin")->value();
            MinRandomPixelIntensity = InputMessage.toInt();
            InputParam   = "IntMin";
            Serial.println("IntMin: " + InputMessage);
        }
        // GET MinBright value on <ESP_IP>/get?MaxBright=<inputMessage>
        else if(request->hasParam("IntMax"))
        {
            InputMessage = request->getParam("IntMax")->value();
            MaxRandomPixelIntensity = InputMessage.toInt();
            InputParam   = "IntMax";
            Serial.println("IntMax: " + InputMessage);
        }
        else
        {
            InputMessage = "No message sent";
            InputParam   = "none";
        }
        request->send(200, "text/html", SendHTML(0));
  });

  server.onNotFound([] (AsyncWebServerRequest *request)
  {
    request->send(404, "text/plain", "404, Non Trouvé");
  });

  #else // !USE_ESP32
    server.on("/",          HandleOnConnect);
    server.on("/Min",       HandleOnConnect);
    server.on("/Party",     HandlePartyIsOn);
    server.on("/PartyDone", HandlePartyIsOff);
    server.on("/MoreParty", HandleMoreParty);
    server.on("/LessParty", HandleLessParty);
    server.onNotFound(HandleNotFound);
  #endif // !USE_ESP32

    server.begin();
    Serial.println("Serveur HTTP Prêt");
}

// ****************************************************************************

#ifndef USE_ESP32

void HandleOnConnect()
{
    Serial.println("Main page!");
    server.send(200, "text/html", SendHTML(0)); 
}

void HandleRX_MinValue()
{
    Serial.println("Rx Min Value");
    server.
}
void HandlePartyIsOn()
{
    Serial.println("Partya Luciola");
    server.send(200, "text/html", SendHTML(1)); 
}

void HandlePartyIsOff()
{
    Serial.println("Partya is done");
    server.send(200, "text/html", SendHTML(0)); 
}

void HandleMoreParty()
{
    Serial.println("More Partya");
    server.send(200, "text/html", SendHTML(2)); 
}

void HandleLessParty()
{
    Serial.println("Less Partya");
    server.send(200, "text/html", SendHTML(0)); 
}

void HandleNotFound()
{
    server.send(404, "text/plain", "404, Non Trouvé");
}
#endif // !USE_ESP32

// ****************************************************************************

String SendHTML(uint8_t Mode)
{
    String ptr;
    String VarValue;
    
    ptr  = "<!DOCTYPE html>";
    ptr += "<html>";
    ptr += "<head>";
    ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">";
    ptr += "<title>Contr&ocirc;le des lucioles</title>";
    ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}<style>";

    ptr += "<style>";
    ptr += CSS_Range;
    ptr += CSS_Submit;
    ptr += CSS_Button;

/*
    
    
    ptr +=".slidecontainer { width: 100%; }";       // Width of the outside container
    
    
    // The slider itself
    ptr +=".slider {";
    ptr +="-webkit-appearance: none;";              // Override default CSS styles
    ptr +="width: 100%;";                           // Full-width
    ptr +="height: 15px;";                          // Specified height
    ptr +="background: #d3d3d3;";                   // Grey background
    ptr +="outline: none;";                         // Remove outline
    ptr +="opacity: 0.7;";                          // Set transparency (for mouse-over effects on hover)
    ptr +="-webkit-transition: .2s;";               // 0.2 seconds transition on hover
    ptr +="transition: opacity .2s;";
    ptr +="}";

    // Mouse-over effects
    ptr +=".slider:hover { opacity: 1; }";          // Fully shown on mouse-over

    // The slider handle (use -webkit- (Chrome, Opera, Safari, Edge) and -moz- (Firefox) to override default look)
    ptr +=".slider::-webkit-slider-thumb {";
    ptr +="-webkit-appearance: none;";              // Override default look
    ptr +="appearance: none;";
    ptr +="width: 25px;";                           // Set a specific slider handle width
    ptr +="height: 25px;";                          // Slider handle height
    ptr +="background: #4CAF50;";                   // Green background
    ptr +="border-radius: 50%;";
    ptr +="box-shadow: 0 0 10px -2px;";
    ptr +="cursor: pointer;";                       // Cursor on hover
    ptr +="}";

    ptr +=".slider::-moz-range-thumb {";
    ptr +="width: 25px;";                           // Set a specific slider handle width
    ptr +="height: 25px;";                          // Slider handle height
    ptr +="border-radius: 50%;";
    ptr +="box-shadow: 0 0 10px -2px;";
    ptr +="background: #4CAF50;";                   // Green background
    ptr +="cursor: pointer;";                       // Cursor on hover
    ptr +="}";
  */  

/*
    // Button
    ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
    ptr +=".button-on {background-color: #1abc9c;}\n";
    ptr +=".button-on:active {background-color: #16a085;}\n";
    ptr +=".button-off {background-color: #34495e;}\n";
    ptr +=".button-off:active {background-color: #2c3e50;}\n";
    ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
*/
    ptr +="</style>\n";
    ptr +="</head>\n";
    ptr +="<body>\n";
    
    //if(Mode != 0)
    //{ 
       // ptr +="<script> var timer = setTimeout(function() { window.location='192.168.1.1/' }, 2000); </script>";
    //}
    
    ptr +="<h1>Lucioles Magiques</h1>\n";
    ptr +="<h3>Copyright (c) Guest who! 2020</h3>\n";
//    ptr +="<h3>Copyright (c) Alain Royer 2020</h3>\n";

    ptr +="<p>Apparition min al&eacute;atoire (5-300).</p>";
    ptr +="<form action=\"/get?Min=\">";
    //ptr +="<label for=\"vol\">(between 5 and 300):</label>";
    ptr +="<input type=\"range\" id=\"myMin\" name=\"Min\" min=\"0\" max=\"50\">";
    ptr +="<input type=\"submit\">";
    ptr +="</form>";
    
    ptr +="<p>Apparition max al&eacute;atoire  (5-300).</p>";
    ptr +="<form action=\"/get?Max=\">";
    //ptr +="<label for=\"vol\">(between 5 and 300):</label>";
    ptr +="<input type=\"range\" id=\"myMax\" name=\"Max\" min=\"0\" max=\"50\">";
    ptr +="<input type=\"submit\">";
    ptr +="</form>";

    ptr +="<p>Intensit&eacute; min al&eacute;atoire (7-13).</p>";
    ptr +="<form action=\"/get?IntMin=\">";
    //ptr +="<label for=\"vol\">(between 5 and 300):</label>";
    ptr +="<input type=\"range\" id=\"myIntMin\" name=\"IntMin\" min=\"0\" max=\"50\">";
    ptr +="<input type=\"submit\">";
    ptr +="</form>";

    ptr +="<p>Intensit&eacute; max al&eacute;atoire (7-13).</p>";
    ptr +="<form action=\"/get?IntMax=\">";
    //ptr +="<label for=\"vol\">(between 5 and 300):</label>";
    ptr +="<input type=\"range\" id=\"myIntMax\" name=\"IntMax\" min=\"0\" max=\"50\">";
    ptr +="<input type=\"submit\">";
    ptr +="</form>";

    /*
    ptr +="<div class=\"slidecontainer\">";
    ptr +="<input type=\"range\" min=\"5\" max=\"300\" value=\"";
    VarValue = String(MinRandomPixelTiming / 1000, DEC);
    ptr += VarValue;
    ptr +="\" class=\"slider\" id=\"myMin\">";
    ptr +="<p>Value: <span id=\"Min\"></span></p>";
    ptr +="</div>";


    ptr +="<p>Apparition max al&eacute;atoire.</p>";
    ptr +="<div class=\"slidecontainer\">";
    ptr +="<input type=\"range\" min=\"5\" max=\"300\" value=\"";
    VarValue = String(MaxRandomPixelTiming / 1000, DEC);
    ptr += VarValue;
    ptr +="\" class=\"slider\" id=\"myMax\">";
    ptr +="<p>Value: <span id=\"Max\"></span></p>";
    ptr +="</div>";

    ptr +="<p>Intensit&eacute; min al&eacute;atoire.</p>";
    ptr +="<div class=\"slidecontainer\">";
    ptr +="<input type=\"range\" min=\"7\" max=\"13\" value=\"";
    VarValue = String(MinRandomPixelIntensity, DEC);
    ptr += VarValue;
    ptr +="\" class=\"slider\" id=\"myIntMin\">";
    ptr +="<p>Value: <span id=\"IntMin\"></span></p>";
    ptr +="</div>";

    ptr +="<p>Intensit&eacute; max al&eacute;atoire.</p>";
    ptr +="<div class=\"slidecontainer\">";
    ptr +="<input type=\"range\" min=\"7\" max=\"13\" value=\"";
    VarValue = String(MaxRandomPixelIntensity, DEC);
    ptr += VarValue;
    ptr +="\" class=\"slider\" id=\"myIntMax\">";
    ptr +="<p>Value: <span id=\"IntMax\"></span></p>";
    ptr +="</div>";

    ptr +="<script>";
    ptr +="var MinHttpReq = XMLHttpRequest();";
    ptr +="var MinSlider = document.getElementById(\"myMin\");";
    ptr +="var MinOutput = document.getElementById(\"Min\");";
    ptr +="MinOutput.innerHTML = MinSlider.value;";
    ptr +="MinSlider.oninput = function() {";

    ptr +="MinOutput.innerHTML = this.value;";
    
    ptr +="MinHttpReq.open(\"GET\", \"get?Min=123\", true)";
    ptr +="MinHttpReq.send(null)";
        
    ptr +="}";
    ptr +="</script>";

    ptr +="<script>";
    ptr +="var MaxSlider = document.getElementById(\"myMax\");";
    ptr +="var MaxOutput = document.getElementById(\"Max\");";
    ptr +="MaxOutput.innerHTML = MaxSlider.value;";
    ptr +="MaxSlider.oninput = function() {";
    ptr +="MaxOutput.innerHTML = this.value;";
    ptr +="}";
    ptr +="</script>";

    ptr +="<script>";
    ptr +="var IntMinSlider = document.getElementById(\"myIntMin\");";
    ptr +="var IntMinOutput = document.getElementById(\"IntMin\");";
    ptr +="IntMinOutput.innerHTML = IntMinSlider.value;";
    ptr +="IntMinSlider.oninput = function() {";
    ptr +="IntMinOutput.innerHTML = this.value;";
    ptr +="}";
    ptr +="</script>";

    ptr +="<script>";
    ptr +="var IntMaxSlider = document.getElementById(\"myIntMax\");";
    ptr +="var IntMaxOutput = document.getElementById(\"IntMax\");";
    ptr +="IntMaxOutput.innerHTML = IntMaxSlider.value;";
    ptr +="IntMaxSlider.oninput = function() {";
    ptr +="IntMaxOutput.innerHTML = this.value;";
    ptr +="}";
    ptr +="</script>";
*/
    if(Mode == 0)
    { 
    }
    


ptr +="<form action=\"/Party\">";
    //ptr +="<label for=\"vol\">(between 5 and 300):</label>";
    ptr +="<input type=\"button\" value=\"Button\">";
    ptr +="<input type=\"button\" value=\"Button\"";
    ptr +="<input type=\"submit\">";
    ptr +="</form>";

   // if(Mode == 1) { ptr +="<p>Partya Luciolas</p><a class=\"button button-off\" href=\"192.168.1.1/Party\">ON</a>\n"; }    // Firework
   // else          { ptr +="<p>normalem Luciolas</p><a class=\"button button-on\" href=\"192.168.1.1/\">OFF</a>\n";               }
    

    ptr +="</body>\n";
    ptr +="</html>\n";
    return ptr;
}

// ****************************************************************************

#endif // USE_WIFI
