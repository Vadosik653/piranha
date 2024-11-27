//
// FirmwareUpdate.cpp
// Обновление ПО
//

#include "FirmwareUpdate.h"
#include "Config.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h>

bool Connected = false;

// Инициализация
void fwu_Init()
{
	Preferences settings;
	settings.begin("wifi-sta", true);
	String SSID = settings.getString("ssid", WIFI_SSID);
	String Pass = settings.getString("pass", WIFI_PASSWORD);
	String Name = settings.getString("name", "");
	settings.end();
	
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
	WiFi.begin(SSID.c_str(), Pass.c_str());
	Serial.printf("WiFi: %s\n", SSID.c_str());
	
	if(Name.length() > 0)
		ArduinoOTA.setHostname(Name.c_str());
	
	ArduinoOTA
	.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

}

static void onConnected()
{
	Serial.print("WiFi connected. IP: ");
	Serial.println(WiFi.localIP());
	ArduinoOTA.begin();
}

static void onDisconnected()
{
	Serial.println("WiFi disconnected");
	ArduinoOTA.end();
}

// Осн. цикл
void fwu_Main()
{
	bool Conn = (WiFi.status() == WL_CONNECTED) || (WiFi.softAPgetStationNum() > 0);
	
	if(Conn != Connected)
	{
		if(Conn) 
			onConnected();
		else
			onDisconnected();
		
		Connected = Conn;
	}
	else
	{
		if(Connected)
		{
			ArduinoOTA.handle();
		}
	}
}
