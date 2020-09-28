/*
vim: set rnu sw=4 ss=4 ts=4 et smartindent autoindent fdm=indent :
Name:        inlineKeyboard.ino
Created:     29/05/2018
Author:      Stefano Ledda <shurillu@tiscalinet.it>
Description: a simple example that do:
             1) if a "show keyboard" text message is received, show the inline custom keyboard,
                otherwise reply the sender with "Try 'show keyboard'" message
             2) if "LIGHT ON" inline keyboard button is pressed turn on the onboard LED and show an alert message
             3) if "LIGHT OFF" inline keyboard button is pressed, turn off the onboard LED and show a popup message
             4) if "see docs" inline keyboard button is pressed,
                open a browser window with URL "https://github.com/shurillu/CTBot"
*/
#include <ESP8266WiFi.h>
#include <aREST.h>
#include <Regexp.h>
#include <Wire.h>                               // DHT12 uses I2C (GPIO4 = SDA, GPIO5 = SCL)
#include "DHT12.h"
#include "MyConfig.h"
#include "Wemospin.h"
#include <Thread.h>
#include <ThreadController.h>
#define LISTEN_PORT 80
#define PIN_RELAY WEMOS_PIN_D5


#define DEBUG

String ssid = WIFI_SSID;     // REPLACE mySSID WITH YOUR WIFI SSID
String pass = WIFI_PASSWORD; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY

aREST rest  = aREST();
WiFiServer server(LISTEN_PORT);


Thread thrRest  = Thread();
Thread thrTimer = Thread();
ThreadController thrContrl  = ThreadController();


signed long __timer;    // in seconds

signed long oldmillis = 0;
signed long get_timeleft() {
    signed long m = millis();
    __timer = __timer - ((m - oldmillis)/1000);
    oldmillis = m;
    if(__timer <= 0)
        __timer = 0;
    return __timer;
}


void thrfRest () {
    WiFiClient client = server.available();
    if(!client) {
        return;
    }
    while(!client.available()){
        delay(1);
    }
    rest.handle(client);
    delay(1);
}

void thrfTimer () {
    static int cnt = 0;
    if(0 >= get_timeleft()) {
        digitalWrite(PIN_RELAY, 1);
        pinMode(PIN_RELAY, INPUT);
        Serial.println("OFF");
    }
#ifdef DEBUG
    Serial.println(String(get_timeleft()));
#endif
    delay(10);
}


void setup() {
    // initialize the Serial
    Serial.begin(115200);
    Serial.println("esp8266 started...");

    rest.set_id("1");
    rest.set_name("esp8266");
    // Function to be exposed
    rest.function("led",ledControl);
    rest.function("status",statusControl);

    // Connect to WiFi
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());

    thrTimer.enabled = true;
    thrTimer.setInterval(6000);
    thrTimer.onRun(thrfTimer);
    thrContrl.add(&thrTimer);

    thrRest.enabled = true;
    thrRest.setInterval(1000);
    thrRest.onRun(thrfRest);
    thrContrl.add(&thrRest);
    __timer = 0;
    
    digitalWrite(PIN_RELAY, 1);
    pinMode(PIN_RELAY, INPUT);
    Serial.println("OFF");
}


void loop () {
    thrContrl.run();
}


// Custom function accessible by the API
int ledControl(String command) {

    // Get state from command
    int  seconds = command.toInt();
    Serial.println("Command:");
    Serial.println(String(seconds));

    if(0 < seconds) {
        __timer = seconds;
        digitalWrite(PIN_RELAY, 0);
        pinMode(PIN_RELAY, OUTPUT);
        Serial.println("ON");
    }
    else {
        __timer = 0;
    }
    return __timer;
}

// Custom function accessible by the API
int statusControl(String command) {
    return __timer;
}
