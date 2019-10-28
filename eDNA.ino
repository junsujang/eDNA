/**
   eDNA Sampler Controller
   @Author: Junsu Jang
   @Date: Oct 24, 2019
   @Descriptions:
    - Control power to the pump (GPIO Output)
    - Read eDNA Sampler ID from RFID/NFC Reader (I2C)
    - Sensor Data:
      - Pressure (I2C)
      - Flowmeter (Interrupt - Falling)
    - RTC for tracking time (I2C)
    - Flash Storage (SPIFFS)
    - Wi-Fi
      - Web communication to export data
      - Set the max-depth profile
    - LED Indicators (power, ready, depth)
*/
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Ticker.h>
#include <FS.h>
#include <NfcAdapter.h>
#include <PN532/PN532/PN532.h>
#include <PN532/PN532_I2C/PN532_I2C.h>
#include "RTClib.h"
#include "MS5837.h"


#define FM_PIN 14
#define PUMP_PIN 12
#define TICK_1S 5000000
#define DEPTH_MARGIN 10 // cm
#define ATM_DEPTH 100   // cm


// RFID
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String eDNA_uid;

// RTC
RTC_DS3231 rtc;
DateTime t_data;

// Pressure
MS5837 p_sensor;
volatile float p_data = 0;
float p_target = 0;

// Flowmeter
volatile uint32_t flow_counter = 0;
volatile uint32_t f_data = 0;

// Flash
String data_file;

Ticker ticker;

//flag
volatile uint8_t fLogData = 0;


// ---------------------------------------------------------------
void data_log() {   //void *pArg
  // Collect data
  cli();
  f_data = flow_counter;
  t_data = rtc.now();
  sei();
  fLogData = 1;
}


// ---------------------------------------------------------------
static void ICACHE_RAM_ATTR isr_flowmeter() {
  flow_counter++;
}


// ---------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  //** WiFi BEGIN **
  WiFi.begin("MIT", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://jsonplaceholder.typicode.com/users");
    http.addHeader("Content-Type", "text/plain");
    int httpCode = http.POST("Message from ESP8266 in FOL");
    String payload = http.getString(); 
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
  }
  //** WiFi END **

  // GPIO for LED Indicators (OUTPUT)

  // GPIO for controlling Power to the pump
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  //** I2C BEGIN **
  Wire.begin();
  // RFID
  nfc.begin();
  // Pressure sensor
  while (!p_sensor.init()) {
    Serial.println("Init failed");
    delay(5000);
  }
  p_sensor.setModel(MS5837::MS5837_30BA);
  p_sensor.setFluidDensity(1029); // kg/m^3

  // Setup RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // ** I2C END **

  // Flowmeter Interrupt Routine
  pinMode(FM_PIN, INPUT_PULLUP);
  attachInterrupt(FM_PIN, isr_flowmeter, FALLING);

  // Data Storage SPIFFS
  SPIFFS.begin();

  // set up the timer @ 1Hz
  ticker.attach_ms(1000, data_log);
}


void loop() {
  p_sensor.read();
  p_data = p_sensor.depth() * 100; // cm

  if (p_data < ATM_DEPTH) {
    // AIR
    // RFID: Need to turn off to re-tag
    if (eDNA_uid == NULL && nfc.tagPresent(500)) {
      NfcTag tag = nfc.read();
      eDNA_uid = tag.getUidString();
      Serial.println(eDNA_uid); //tag.print();
    }
    // Bluetooth
//    if ((WiFi.status() != WL_CONNECTED) && (btSerial.available() > 0)) {
//      String ssid = btSerial.readStringUntil('\r\n');
//      ssid.remove(length(ssid)-2, 2);
//      String pwd = btSerail.readStringUntil('\r\n');
//      pwd.remove(length(ssid)-2, 2);
//      // WiFi
//      WiFi.begin(ssid, pwd);
//      while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//      }
//      btSerial.print("Connected, IP address: ");
//      btSerial.println(WiFi.localIP());
//    }
    // Web Service
//    if (WiFi.status() == WL_CONNECTED) {
//      // We are in air and connected to wifi
        // Upload the data if I stored it
        // if not, 
//      HTTPCLient http;
//      http.begin("http://jsonplaceholder.typicode.com/users");
//      http.addHeader("Content-Type", "text/plain");
//      int httpCode = http.POST("Message from ESP8266 in FOL");
//      http.end();
//    }
  } else {
    // WATER
    //  // JUNSU BEGIN
    //  // We need to know when to start and stop pumping water in
    //  if (abs(p_data - p_target) < DEPTH_MARGIN) {
    //    digitalWrite(PUMP_PIN, HIGH);
    //  } else {
    //    digitalWrite(PUMP_PIN, LOW);
    //  }
    //  // JUNSU END
//    if (fLogData == 1) {
//      // Format data
//      String data_wrapper = data_wrap_str(t_data, p_data, f_data, eDNA_uid)
//                            // Open file and save
//      if (SPIFFS.exists(data_file) {
//      File f = SPIFFS.open(data_file, "a+");
//        int bytesWritten = f.println(data_wrapper);
//        f.close();
//      }
//    }
  }
}
