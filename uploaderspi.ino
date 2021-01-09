#include<FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
String hname = "";
bool uploadCodeInAtmega = false;
void setup() {
  // put your setup code here, to run once:
  hname = "NodeAvrisp";
  Serial.begin(115200, SERIAL_8N1);
  //setup1();
  initEspRadio();
  MDNS.begin(hname);
  initLocalServer();
  initRadioControllerUpdater();
  initHardwareControllerUpdater();
  startDebugOverAir();
  SPIFFS.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  localServerUtil() ;
  debuggingUtil();
  //loop1();
  radioControllerUpdaterUtil();
}
