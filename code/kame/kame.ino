//add to config and use boardmanager: http://arduino.esp8266.com/stable/package_esp8266com_index.json
//install via library manager: Adafruit MQTT
//download and install zip (don't use library manager!) https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "minikame.h"

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "galway"
#define WLAN_PASS       "..."

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
//#define AIO_SERVER      "192.168.2.126"

#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "bohne"
#define AIO_KEY         "c055c20e62914fc5861e08c9cfa36189"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe minikame1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/minikame1");

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();






void parseData(String data);

MiniKame robot;
WiFiServer server(80);
bool running = 0;
String input;

uint32_t x = 0;



void setup() {
  Serial.begin(115200);
  Serial.println("minikame");

  pinMode(D7, OUTPUT);//status LED blue

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&minikame1);

  delay(1000);
  robot.init();
}

void _loop()
{
  if (running) {
  }
  else
  {

    delay(200);
    Serial.print(".");
    //robot.home();
    //robot.upDown(4,250);
    robot.walk(1, 550);
  }
}

void loop() {

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &minikame1)
    {
      Serial.print("Recieved: ");
      Serial.println((char *)minikame1.lastread);
      parseData((char *)minikame1.lastread);
    }
  }

  if (!mqtt.ping()) {
    mqtt.disconnect();
  }

}

void parseData(String data) {

  digitalWrite(D7, HIGH);
  switch (data.toInt()) {

    case 1: // Up
      Serial.println("walking");
      robot.walk(1, 550);
      running = 1;
      break;

    case 2: // Down
    Serial.println("walk 10 steps");
      robot.walk(10, 550);
      running = 1;
      break;

    case 3: // Left
      Serial.println("left");
      robot.turnL(1, 550);
      running = 1;
      break;

    case 4: // Right
    Serial.println("right");
      robot.turnR(1, 550);
      running = 1;
      break;

    case 5: // STOP
      Serial.println("stop");
      running = 0;
      break;

    case 6: // heart
      Serial.println("pushUp");
      robot.pushUp(2, 2000);
      break;

    case 7: // fire
      Serial.println("upDown");
      robot.upDown(4, 250);
      break;

    case 8: // skull
      Serial.println("jump");
      robot.jump();
      break;

    case 9: // cross
      Serial.println("hello");
      robot.hello();
      break;

    case 10: // punch
      Serial.println("frontBack");
      robot.frontBack(4, 200);
      break;

    case 11: // mask
      Serial.println("dance");
      robot.dance(2, 1000);
      break;

    default:
      Serial.println("home");
      robot.home();
      break;
  }
  digitalWrite(D7, LOW);
}



// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
