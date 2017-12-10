//Version 0.1 first Testversion for displaying a Bitmap

#define FASTLED_ALLOW_INTERRUPTS 0
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>



#include <SPI.h>
#include <Wire.h>

//Puffer für Bitmap
#define LEDS 60
#define COLLNUMS LEDS * 3
#define RGB 3
#define BUFFERSIZE LEDS * COLLNUMS * RGB

CRGB leds[60];

//WLAN Zugang
const char *ssid = "WLAN";
const char *password = "12345678";
ESP8266WebServer server ( 80 );

//Buffer für Bitmap und Variablen zur befüllung
char *bitmap = NULL;

long buffercounter = 1;

long bytecounter = 0;

boolean bitmapReady = false;

void setup ( void ) {

  FastLED.addLeds<WS2812B, 1 >(leds, 60);

  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
  }
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  server.onFileUpload(upload);
  server.on ( "/upload", upload );
  server.on( "/start", writebitmap);
  server.on("/setup", setParameter);
  server.on("/", handleRoot);
  server.begin();

}

void loop ( void ) {
  server.handleClient();
}

void handleRoot() {

  String index = "<html><head><style type='text/css'>a{font: bold 20em Arial;text-decoration: none;background-color: #EEEEEE;color: #333333;padding: 2px 6px 2px 6px;border-top: 1px solid #CCCCCC;}</style></head><a href='/start'>Start</a></html>";
    server.send ( 200, "text/html", index );
  }

void setParameter() {
Serial.print("Paramter: ");
Serial.println(server.args());
int i = 0;
for (i = 0; i < server.args(); i++){
  
  if (strcmp(server.argName(i).c_str(),"black")){
    int x =  atoi(server.arg(i).c_str());
     Serial.printf("Black[%i]: %s: %i\n", i, server.argName(i).c_str(),x );
  }
  Serial.printf("Paramter[%i]: %s: %s\n", i, server.argName(i).c_str(), server.arg(i).c_str());
}
   server.send ( 200, "text/plain", "Okay" );
}

void upload() {

  HTTPUpload& upload = server.upload();

  int c = 0;

  switch (upload.status)
  {
    case UPLOAD_FILE_START:
      bitmapReady = false;
      if (bitmap != NULL) {
        free(bitmap);
      }
      Serial.print("Erzeuge Puffer");
      bitmap = (char*)calloc(BUFFERSIZE, sizeof(char));
      Serial.printf("index heap size: %u\n", ESP.getFreeHeap());

      if (bitmap == NULL) {
        server.send ( 400, "text/plain", "error" );
        Serial.print("Error");
        break;
      }
      bytecounter = 0;
      break;

    case UPLOAD_FILE_WRITE:

      for (c = 0 ; c < (bytecounter + upload.currentSize); c++) {
        bitmap[c + bytecounter] = upload.buf[c];
      }
      bytecounter += upload.currentSize;
      break;


    case UPLOAD_FILE_END:
   /*   String d="";
      for(int c = 0 ; c < 180 ; c++) d+=(char)bitmap[c];
        Serial.print(d); */
      bitmapReady = true;
      server.send ( 200, "text/plain", "okay" );
      //Buffer muss vielfaches von LED Leiste sein >> 200 okay
      //sonst 400 passt nicht

      break;

  }

}


void writebitmap() {

  long z = 0;
  long zz;
  long c;
  if ( bitmapReady) {
    server.send ( 200, "text/plain", "Run" );

    for (c = 0 ; c < bytecounter ;) {
      zz = z % LEDS;
      leds[zz].r = bitmap[c];
      c++;
      leds[zz].g = bitmap[c];
      c++;
      leds[zz].b = bitmap[c];
      c++;
      z++;
      if (z == 60 ) {
        z = 0;
        FastLED.show();
        delay(50);
        for (int k = 0; k < 60 ; k++) {
          leds[k] = CRGB::Black;
        }
        FastLED.show();
        delay(20);
      } 
    } 
  } else {
    server.send ( 400, "text/plain", "Bitmap nicht geladen" );
  }
}
