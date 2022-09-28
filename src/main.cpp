#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define WEBSERVER_H
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266mDNS.h>
#include <string.h>

#include "blink.h"
#include "lamp.h"
#include "led.h"
#include "PubSubClient.h"

const char *apName = "light_wifi_config";
const char *hostName = "mylight";

#define ID_MQTT  "d6d8fcd160ce48a3b38ff76e7e2df726"     //用户私钥，控制台获取
const char*  topic = "light002";        //主题名字，可在巴法云控制台自行创建，名称随意

const char* mqtt_server = "bemfa.com"; //默认，MQTT服务器
const int mqtt_server_port = 9501;      //默认，MQTT服务器
WiFiClient espClient;
PubSubClient client(espClient);

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

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Topic:");
	Serial.println(topic);
	String msg = "";
	for (unsigned int i = 0; i < length; i++) {
		msg += (char)payload[i];
	}
	Serial.print("Msg:");
	Serial.println(msg);
	if (msg == "off") {
		led_update_v(0);
	} else if (msg == "on") {
		led_update_v(100);
	} else {
		msg.replace("on#", "");
		led_update_v(atoi(msg.c_str()));
	}
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID_MQTT)) {
      Serial.println("connected");
      Serial.print("subscribe:");
      Serial.println(topic);
      //订阅主题，如果需要订阅多个主题，可发送多条订阅指令client.subscribe(topic2);client.subscribe(topic3);
      client.subscribe(topic);
	  blink_ok();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
	  blink_err();
      delay(5000);
    }
  }
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
		led_init();
		blink_ok();
		start_server();
		client.setServer(mqtt_server, mqtt_server_port); // 设置mqtt服务器
  		client.setCallback(callback); // mqtt消息处理
	}
}

void loop() {
	// put your main code here, to run repeatedly:
	MDNS.update();
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
}