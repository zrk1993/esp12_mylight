#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define WEBSERVER_H
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>

#include "blink.h"
#include "lamp.h"
#include "led.h"

const char *apName = "light_wifi_config";
const char *hostName = "mylight";

WiFiManager wm;
AsyncWebServer server(80);

void start_server() {
	if (MDNS.begin(hostName)) { // Start mDNS with name esp8266
		Serial.println("mDNS started");
		Serial.printf("http://%s.local\n", hostName);
	}

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", lamp_html_gz, lamp_html_gz_len);
		response->addHeader("Content-Encoding", "gzip");
		request->send(response);
	});
	server.on("/led_bri", HTTP_GET, [](AsyncWebServerRequest *request) {
		int i = get_light_bri();
		char c[7];
		itoa(i,c,10);
		Serial.println(i);
		Serial.println(c);
		request->send(200, "text/plain", c);
	});
	server.on("/lamp", HTTP_GET, [](AsyncWebServerRequest *request) {
		if(request->hasArg("v")) {
			String v = request->arg("v");
			led_update_v(atoi(v.c_str()));
			request->send(200, "text/plain", "Hi! I am ESP8266 lamp. " + v);
		} else {
			request->send(200, "text/plain", "Hi! I am ESP8266 lamp.");
		}
	});
	server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/plain", "reset");
		wm.resetSettings();
		Serial.println("restart 。。。 ");
		ESP.restart();
	});

	AsyncElegantOTA.begin(&server); // Start ElegantOTA
	server.begin();
	Serial.println("HTTP server started");
}

bool shouldSaveConfig = false;
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
	Serial.begin(9600);

	// reset settings - wipe stored credentials for testing
	// these are stored by the esp library

	wm.setSaveConfigCallback(saveConfigCallback);

	bool res = wm.autoConnect(apName);
	if (!res) {
		Serial.println("Failed to connect");
		blink_err();
		wm.resetSettings();
		// ESP.restart();
	} else {
		if (shouldSaveConfig) {
			Serial.println("restart ...");
			blink_ok();
			ESP.restart();
		}
		// if you get here you have connected to the WiFi
		Serial.println("connected...yeey :)");
		Serial.println("IP address: ");
		Serial.println(WiFi.localIP());
		blink_ok();
		start_server();
		led_init();
	}
}

void loop() {
	// put your main code here, to run repeatedly:
	MDNS.update();
}