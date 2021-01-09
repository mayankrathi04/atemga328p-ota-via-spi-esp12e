#include<stdlib.h>
#include <ESP8266WebServer.h>
//#include <ESP8266WebServerSecure.h>
#include <ESP8266HTTPUpdateServer.h>
const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "qwerty123.";
//BearSSL::ESP8266WebServerSecure httpServer(443);
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
File fsUploadFile;
/*
  static const char serverCert[] PROGMEM = R"EOF(
  -----BEGIN CERTIFICATE-----
  MIIDSzCCAjMCCQD2ahcfZAwXxDANBgkqhkiG9w0BAQsFADCBiTELMAkGA1UEBhMC
  VVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDU9yYW5nZSBDb3VudHkx
  EDAOBgNVBAoMB1ByaXZhZG8xGjAYBgNVBAMMEXNlcnZlci56bGFiZWwuY29tMR8w
  HQYJKoZIhvcNAQkBFhBlYXJsZUB6bGFiZWwuY29tMB4XDTE4MDMwNjA1NDg0NFoX
  DTE5MDMwNjA1NDg0NFowRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3Rh
  dGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZI
  hvcNAQEBBQADggEPADCCAQoCggEBAPVKBwbZ+KDSl40YCDkP6y8Sv4iNGvEOZg8Y
  X7sGvf/xZH7UiCBWPFIRpNmDSaZ3yjsmFqm6sLiYSGSdrBCFqdt9NTp2r7hga6Sj
  oASSZY4B9pf+GblDy5m10KDx90BFKXdPMCLT+o76Nx9PpCvw13A848wHNG3bpBgI
  t+w/vJCX3bkRn8yEYAU6GdMbYe7v446hX3kY5UmgeJFr9xz1kq6AzYrMt/UHhNzO
  S+QckJaY0OGWvmTNspY3xCbbFtIDkCdBS8CZAw+itnofvnWWKQEXlt6otPh5njwy
  +O1t/Q+Z7OMDYQaH02IQx3188/kW3FzOY32knER1uzjmRO+jhA8CAwEAATANBgkq
  hkiG9w0BAQsFAAOCAQEAnDrROGRETB0woIcI1+acY1yRq4yAcH2/hdq2MoM+DCyM
  E8CJaOznGR9ND0ImWpTZqomHOUkOBpvu7u315blQZcLbL1LfHJGRTCHVhvVrcyEb
  fWTnRtAQdlirUm/obwXIitoz64VSbIVzcqqfg9C6ZREB9JbEX98/9Wp2gVY+31oC
  JfUvYadSYxh3nblvA4OL+iEZiW8NE3hbW6WPXxvS7Euge0uWMPc4uEcnsE0ZVG3m
  +TGimzSdeWDvGBRWZHXczC2zD4aoE5vrl+GD2i++c6yjL/otHfYyUpzUfbI2hMAA
  5tAF1D5vAAwA8nfPysumlLsIjohJZo4lgnhB++AlOg==
  -----END CERTIFICATE-----
  )EOF";

  static const char serverKey[] PROGMEM =  R"EOF(
  -----BEGIN RSA PRIVATE KEY-----
  MIIEpQIBAAKCAQEA9UoHBtn4oNKXjRgIOQ/rLxK/iI0a8Q5mDxhfuwa9//FkftSI
  IFY8UhGk2YNJpnfKOyYWqbqwuJhIZJ2sEIWp2301OnavuGBrpKOgBJJljgH2l/4Z
  uUPLmbXQoPH3QEUpd08wItP6jvo3H0+kK/DXcDzjzAc0bdukGAi37D+8kJfduRGf
  zIRgBToZ0xth7u/jjqFfeRjlSaB4kWv3HPWSroDNisy39QeE3M5L5ByQlpjQ4Za+
  ZM2yljfEJtsW0gOQJ0FLwJkDD6K2eh++dZYpAReW3qi0+HmePDL47W39D5ns4wNh
  BofTYhDHfXzz+RbcXM5jfaScRHW7OOZE76OEDwIDAQABAoIBAQDKov5NFbNFQNR8
  djcM1O7Is6dRaqiwLeH4ZH1pZ3d9QnFwKanPdQ5eCj9yhfhJMrr5xEyCqT0nMn7T
  yEIGYDXjontfsf8WxWkH2TjvrfWBrHOIOx4LJEvFzyLsYxiMmtZXvy6YByD+Dw2M
  q2GH/24rRdI2klkozIOyazluTXU8yOsSGxHr/aOa9/sZISgLmaGOOuKI/3Zqjdhr
  eHeSqoQFt3xXa8jw01YubQUDw/4cv9rk2ytTdAoQUimiKtgtjsggpP1LTq4xcuqN
  d4jWhTcnorWpbD2cVLxrEbnSR3VuBCJEZv5axg5ZPxLEnlcId8vMtvTRb5nzzszn
  geYUWDPhAoGBAPyKVNqqwQl44oIeiuRM2FYenMt4voVaz3ExJX2JysrG0jtCPv+Y
  84R6Cv3nfITz3EZDWp5sW3OwoGr77lF7Tv9tD6BptEmgBeuca3SHIdhG2MR+tLyx
  /tkIAarxQcTGsZaSqra3gXOJCMz9h2P5dxpdU+0yeMmOEnAqgQ8qtNBfAoGBAPim
  RAtnrd0WSlCgqVGYFCvDh1kD5QTNbZc+1PcBHbVV45EmJ2fLXnlDeplIZJdYxmzu
  DMOxZBYgfeLY9exje00eZJNSj/csjJQqiRftrbvYY7m5njX1kM5K8x4HlynQTDkg
  rtKO0YZJxxmjRTbFGMegh1SLlFLRIMtehNhOgipRAoGBAPnEEpJGCS9GGLfaX0HW
  YqwiEK8Il12q57mqgsq7ag7NPwWOymHesxHV5mMh/Dw+NyBi4xAGWRh9mtrUmeqK
  iyICik773Gxo0RIqnPgd4jJWN3N3YWeynzulOIkJnSNx5BforOCTc3uCD2s2YB5X
  jx1LKoNQxLeLRN8cmpIWicf/AoGBANjRSsZTKwV9WWIDJoHyxav/vPb+8WYFp8lZ
  zaRxQbGM6nn4NiZI7OF62N3uhWB/1c7IqTK/bVHqFTuJCrCNcsgld3gLZ2QWYaMV
  kCPgaj1BjHw4AmB0+EcajfKilcqtSroJ6MfMJ6IclVOizkjbByeTsE4lxDmPCDSt
  /9MKanBxAoGAY9xo741Pn9WUxDyRplww606ccdNf/ksHWNc/Y2B5SPwxxSnIq8nO
  j01SmsCUYVFAgZVOTiiycakjYLzxlc6p8BxSVqy6LlJqn95N8OXoQ+bkwUux/ekg
  gz5JWYhbD6c38khSzJb0pNXCo3EuYAVa36kDM96k1BtWuhRS10Q1VXk=
  -----END RSA PRIVATE KEY-----
  )EOF";
*/
void initLocalServer() {
  //httpServer.setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.onNotFound([]() {
    httpServer.send(404, "text/plain", "FileNotFound");
  });
  httpServer.on("/", HTTP_GET, []() {
    printDebugData("Serving Page / \r\n");
    httpServer.send(200, "text/html", F("<html><head><title>Node Local Server</title></head><body align='center'><button id='setTime'>Set Time</button><br><button id='uploadCodeAtmega'>Upload Code Atmega</button><br><button id='uploadCodeEsp'>Upload Code Esp</button><script type='text/javascript'>document.getElementById('uploadCodeAtmega').onclick = function () {location.href = '/atmega';};document.getElementById('uploadCodeEsp').onclick = function () {location.href = '/firmware';};document.getElementById('setTime').onclick = function () {location.href = '/setTime';};</script></body></html>"));
  });
  httpServer.on("/setTime", handleTime);
  httpServer.on("/atmega", HTTP_POST, []() {
    httpServer.send(200);
    printDebugData("Serving Page /atmega/upload\r\n");
  }, handleFileUpload);
  httpServer.on("/atmega", HTTP_GET, []() {
    printDebugData("Serving Page /atmega\r\n");
    httpServer.send(200, "text/html", F("<!DOCTYPE html><html><head></head><body><form method=\"post\" enctype=\"multipart/form-data\"><input type=\"file\" name=\"name\"><input class=\"button\" type=\"submit\" value=\"Upload\"></form></body></html>"));
  });
  httpServer.begin();
  printDebugData("HTTP server started");
}
void handleTime() {
  printDebugData("Setting time ");
  if (httpServer.args() == 0) {
    printDebugData("Serving Page /setTime \r\n");
    httpServer.send(200, "text/html", F("<!DOCTYPE html><html><head><title>SetTime</title><script type='text/javascript'>function myFunction(){var ms = ((new Date).getTime())/1000;var s = ms - (946665000);document.getElementById('getTime').value = Math.round(s);}</script></head><body><input type='button' name='Time' value='Get Time' onclick='myFunction()'><form method=\"post\" enctype=\"multipart/form-data\"><input type='number' id='getTime' name='time'><br><input type='submit' value='Update Time'></form></body></html>"));
  }
  else {
    /*
    String t = httpServer.arg(0);
    uint32_t tt = strtoul(t.c_str(), NULL, 10);
    printDebugData(String(tt));
    ApplicationPacket ap;
    ap.msgPack[0] = (tt >> 24) & 0xFF;
    ap.msgPack[1] = (tt >> 16) & 0xFF;
    ap.msgPack[2] = (tt >> 8) & 0xFF;
    ap.msgPack[3] = tt & 0xFF;
    sendInfoToHardwareController(ap, 139);
    httpServer.send(200, "text/plain", "1");
    */
  }
}
void handleFileUpload() { // upload a new file to the SPIFFS
  HTTPUpload& upload = httpServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    printDebugData("handleFileUpload Name: " + filename + "\r\n");
    fsUploadFile = SPIFFS.open("hardwareFirmware.hex", "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
  }
  else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  }
  else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {                                   // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      printDebugData("handleFileUpload Size: ");
      printDebugData(String(upload.totalSize) + "\r\n");
      uploadCodeInAtmega = true;
      bool res = hardwareControllerUpdaterUtil();
      httpServer.send(200, "text/plain", String(res));
    }
    else {
      httpServer.send(500, "text/plain", "500: couldn't create file");
    }
  }
}
void localServerUtil() {
  httpServer.handleClient();
}
