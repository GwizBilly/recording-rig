/*
  Hello, world!
*/
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
#define PIN D2
#define TOUCH_PIN D7
int touchVal = 0;

#define USE_SERIAL Serial
int rec = 0;
Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      //webSocket.sendTXT("Connected");
    }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);
      if (payload[0] = 'd') {
        rec = 0;
      }
      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
        case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
    }

}

void setup() {
  pinMode(TOUCH_PIN, INPUT);
  pixels.begin();
  delay(400);
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.setPixelColor(0, pixels.Color(1, 20, 3));
  pixels.show();   // Send the updated pixel colors to the hardware.
  delay(400);
  Serial.begin(115200);

  WiFiMulti.addAP("", "");

  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // server address, port and URL
  webSocket.begin("192.xxx.x.xx", 8080, "/");

  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  //webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);
}

void loop() {
  touchVal = digitalRead(TOUCH_PIN);
  pixels.clear();
  if (touchVal == true) {
    USE_SERIAL.println("Touch");
    pixels.setPixelColor(0, pixels.Color(254, 254, 254));
    pixels.show();
    delay(1000);
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(0, 254, 0));
    pixels.show();
    delay(5000);
    rec = 1;
    webSocket.sendTXT("Start recording!");
  } else {
    if (rec == 1) {
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    } else {
      pixels.setPixelColor(0, pixels.Color(0, 0, 1));
      rec = 0;
    }
  }
  pixels.show();
  webSocket.loop();
}
