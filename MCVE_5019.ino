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

#define RTC_TEST 1537577487 // GMT Epoch see https://www.epochconverter.com/


// *******************************************************************************************
void setupLocalTime()
// *******************************************************************************************
{
	// see https://github.com/esp8266/Arduino/issues/4637
	time_t now;
	now = time(nullptr);
	Serial.print("Initial time:"); Serial.println(now);
	Serial.println(ctime(&now));

	int myTimezone = -7;
	int dst = 0;
	int SecondsPerHour = 3600;
	if (now <= 1500000000) {
		// try to set network time via ntp packets
		configTime(myTimezone * SecondsPerHour, dst * 0, "pool.ntp.org", "time.nist.gov");
		setenv("TZ", "PST8PDT,M4.1.0/02:00:00,M10.5.0/02:00:00", 1); // see https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
		tzset();
		Serial.println("\nWaiting for time");
		while (!time(nullptr)) {
			Serial.print(".");
			delay(1000);
		}
	}
	
	// get the time from the system
	now = time(nullptr);
	Serial.print("Next time:");  Serial.println(now);
	Serial.println(ctime(&now));

	// some networks may not allow ntp protocol (e.g. guest networks) so we may need to fudge the time
	if (now <= 1500000000) {
		// set to RTC text value
		// see https://www.systutorials.com/docs/linux/man/2-settimeofday/
		//
		//struct timeval {
		//	time_t      tv_sec;     /* seconds */
		//	suseconds_t tv_usec;    /* microseconds */
		//};
		timeval tv = { RTC_TEST, 0 };
		//
		//struct timezone {
		//	int tz_minuteswest;     /* minutes west of Greenwich */
		//	int tz_dsttime;         /* type of DST correction */
		//};
		timezone tz = { myTimezone * 60 , 0 };  

		// int settimeofday(const struct timeval *tv, const struct timezone *tz);
		settimeofday(&tv, &tz);
	}

	now = time(nullptr);
	Serial.println("Final time:");  Serial.println(now);
	Serial.println(ctime(&now));
}

// *******************************************************************************************
// 
// *******************************************************************************************
void setup() 
{
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
//
// *******************************************************************************************
void loop()
{
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
