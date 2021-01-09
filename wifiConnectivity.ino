String wMode = "WIFI";
const char* wSsid     = "****";
const char* wPassword = "****";
const char* hSsid     = "espme";
const char* hPassword = "abc123abc";
IPAddress myIP;
void initEspRadio(){
  if(wMode=="WIFI"){
    connectToWifi();
    WiFi.hostname(hname);
  }
  else{
    createHotspot();
  }
}
void connectToWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wSsid, wPassword);
  wMode = "WIFI";
}
void createHotspot() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(hSsid, hPassword);
  wMode = "HOTSPOT";
}
void turnOffWireless(){
  
}
void turnOnWireless(){
  
}
void loadIp() {
  if (wMode == "HOTSPOT")
    myIP = WiFi.softAPIP();
  else if (wMode == "WIFI" && WiFi.status() == WL_CONNECTED)
    myIP = WiFi.localIP();
  else{
    myIP[0] = 0;
    myIP[1] = 0;
    myIP[2] = 0;
    myIP[3] = 0;
  }
}
