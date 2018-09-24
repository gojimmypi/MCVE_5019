#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#include "GlobalDefine.h"
#include "WiFiHelper.h"
#include "htmlHelper.h"

#include <sys/time.h>                   // struct timeval
#include <time.h>

BearSSL::WiFiClientSecure  client;
String DasboardDataFile = DASHBOARD_DEFAULT_DATA; // set a default, but based on mac address we might determine a user-specific value
const int httpPort = 443;


//*******************************************************************************************************************************************
// setupWiFi
//*******************************************************************************************************************************************
int setupWiFi() {
	SET_HEAP_MESSAGE("setupWiFi Heap = ");
	HEAP_DEBUG_PRINTLN("Heap (setupWiFi begin)");

#ifdef ARDUINO_ARCH_ESP8266
	client.setInsecure(); // TODO fix this. Needed for BearSSL
#endif

#ifdef ARDUINO_ARCH_ESP32
						  //client.setInsecure(); // not implemented in ESP32
#endif
	HEAP_DEBUG_PRINTLN("htmlSetClient(&client)...");
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	htmlSetClient(&client);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	WIFI_DEBUG_PRINTLN("calling wifiConnect...");
	wifiConnect(50);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);


	WIFI_DEBUG_PRINTLN("WiFi connected. My IP address:");
	WIFI_DEBUG_PRINTLN("");
	WIFI_DEBUG_PRINTLN(WiFi.localIP());
	if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
		Serial.println("Successfully connected!");
	}

	// testSSL();

	HEAP_DEBUG_PRINT("Heap (setupWiFi end); "); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	return 0;
}

#define RTC_TEST 1537642676 // see https://www.epochconverter.com/


// *******************************************************************************************
void setupLocalTime()
// *******************************************************************************************
{
	time_t now;
	now = time(nullptr);
	Serial.print("Initial time:"); Serial.println(now);
	Serial.println(ctime(&now));

	int myTimezone = -8;
	int dst = 0;

	if (now <= 1500000000) {
		// try to set network time. Note some guest networks may not allow ntp!
		struct tm timeinfo;
		configTime(myTimezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
		Serial.println("\nWaiting for time");
		while (!time(nullptr)) {
			Serial.print(".");
			delay(1000);
		}
	}
    delay(5000); // allow a few seconds to connect to network time.

	now = time(nullptr);
	Serial.print("Next time:");  Serial.println(now);
	Serial.println(ctime(&now));

	if (now <= 1500000000) {
		// set to RTC text value
		timeval tv = { RTC_TEST, 0 };
		timezone tz = { 0 , 0 };
		settimeofday(&tv, &tz);
		setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);
		tzset();
	}

	now = time(nullptr);
	Serial.println("Final time:");  Serial.println(now);
	Serial.println(ctime(&now));
}

// *******************************************************************************************
// *******************************************************************************************
void setup() {
	delay(500);
	Serial.begin(115200);
	Serial.println("MCVE 5029 V1");
	

	//configTime(0, 0, "pool.ntp.org");
	//setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);
	//tzset();

	// sntp_set_system_time(0);
	// configTime(3600, 3600, "pool.ntp.org");
	setupLocalTime();

	setupWiFi();
	
	Serial.println(">> setup complete! ");

	Serial.println("");
	Serial.println("");
	Serial.println("");


}

// *******************************************************************************************
// *******************************************************************************************
void loop() {
	client.flush();
	client.stopAll();
	client.setInsecure();
	if (!client.connect(DASHBOARD_HOST, httpPort)) {
		Serial.println("connection failed; need to implement wait clear"); // TODO 
		Serial.println("");
		Serial.print("client status = ");
		Serial.println(client.status());
		Serial.print("client available = ");
		Serial.println(client.available());
		Serial.print("client getLastSSLError = ");
		Serial.println(client.getLastSSLError());
		Serial.print("client getWriteError = ");
		Serial.println(client.getWriteError());
		Serial.print("client getWriteError = ");
	}
	else {
		Serial.println("Success!"); // We never get here if first needing to accept terms and conditions, instead: error 
	}
	yield();
	delay(60000); 
	time_t now = time(nullptr);
	Serial.println(ctime(&now));

}
