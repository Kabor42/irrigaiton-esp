#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <AceTimeClock.h>
#include <AceTime.h>
#include <uri/UriBraces.h>

#define WIFI_SSID "NagyNyeste"
#define WIFI_PASS "Fenetuggya001"
#define WEB_SERVER_PORT 8080

ESP8266WebServer webServer(WEB_SERVER_PORT);

static ace_time::BasicZoneProcessor cetProcessor;
static ace_time::clock::NtpClock ntpClock;

struct Schedule
{
  byte days;
  byte zone;
  uint16_t begin;
  uint16_t duration;
  long long start;
  long long stop;
};

Schedule tmp[30];
int zone = 0;

void handleShcedule()
{
  ace_time::acetime_t now = ntpClock.getNow();
  ace_time::TimeZone cetTz = ace_time::TimeZone::forZoneInfo(&ace_time::zonedb::kZoneEurope_Budapest, &cetProcessor);
  ace_time::ZonedDateTime cetTime = ace_time::ZonedDateTime::forEpochSeconds(now, cetTz);

  DynamicJsonDocument doc(4096);
  doc["time"] = cetTime.toUnixSeconds();
  JsonArray array = doc.createNestedArray("schedules");
  for (byte i = 0; i < 30; ++i)
  {
    JsonObject doc = array.createNestedObject();
    doc["days"] = tmp[i].days;
    doc["zones"] = tmp[i].zone;
    doc["begin"] = tmp[i].begin;
    doc["dur"] = tmp[i].duration;
    doc["start"] = tmp[i].start;
    doc["stop"] = tmp[i].stop;
  }
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

void handleZone() {
  ace_time::acetime_t now = ntpClock.getNow();
  ace_time::TimeZone cetTz = ace_time::TimeZone::forZoneInfo(&ace_time::zonedb::kZoneEurope_Budapest, &cetProcessor);
  ace_time::ZonedDateTime cetTime = ace_time::ZonedDateTime::forEpochSeconds(now, cetTz);

  DynamicJsonDocument doc(128);
  doc["time"] = cetTime.toUnixSeconds();
  JsonArray array = doc.createNestedArray("zones");
  for(byte i=0; i < 8; ++i) {
    JsonObject obj = array.createNestedObject();
    obj["value"] = (zone & (1<<i)) ? true : false; 
  }
  String response;
  serializeJson(doc, response);
  webServer.send(200, "application/json", response);
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "Uri: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (int i = 0; i < webServer.args(); ++i)
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  webServer.send(404, "text/plain", message);
}

void setup()
{
  Serial.begin(115200);
  LittleFS.begin();

  File fp = LittleFS.open("/tmp.bin", "r");
  for (byte i = 0; i < 30; ++i)
    fp.read((byte *)&(tmp[i]), sizeof(Schedule));
  fp.close();

  Serial.println("[WiFi] Setup");
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("[WiFi Connected!");
  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("[WebServer] Setup");
  webServer.on("/api/schedule", handleShcedule);
  webServer.on("/api/zone", handleZone);
  webServer.on(UriBraces("/api/zone/{}"), [](){
    String zonestr = webServer.pathArg(0);
    webServer.send(200, "text/plain", "Toggle zone: '" + zonestr + "'");
  });
  webServer.on(UriBraces("/api/schedule/{}"), [](){
    String sched_str = webServer.pathArg(0);
    webServer.send(200, "text/plain", "Modify Schedule #: " + sched_str);
  });
  webServer.onNotFound(handleNotFound);
  Serial.println("[WebServer] Starting..");
  webServer.begin();
  Serial.println("[WebServer] Running.");

  ntpClock.setup();
  if(!ntpClock.isSetup()) {
    Serial.println("Something went wrong.");
  }
}

void loop()
{
  webServer.handleClient();
}