#ifndef REQUESTPARSER_H
#define REQUESTPARSER_H
#include <Arduino.h>
#include <WiFi101.h>

class RequestParser {
 public:
  RequestParser(WiFiClient client);
  void addHeaderCharacter(char c);
  String getHeader();
  void grabPayload();
  String getPayload();
  String getField(String key);

 private:
  String _header;
  String _payload;
  WiFiClient _client;

  String getHeaderField(String data, String key);
  String getLine(String data);
  String popLine(String data);
  String getLineFromIndex(String data, int index);
  String extractPayload(String payload, String key);
};

#endif
