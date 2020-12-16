#include "RequestParser.h"

String getLine(String data) {
  int endOfLineIndex = data.indexOf("\r\n");
  return data.substring(0, endOfLineIndex);
}

String popLine(String data) {
  int endOfLineIndex = data.indexOf("\r\n");
  // Serial.println(data.substring(endOfLineIndex + 2, data.length() - 1));
  return data.substring(endOfLineIndex + 2, data.length());
}

String getHeaderField(String data, String key) {
  int keyIndex = data.indexOf(key);
  if (keyIndex == -1) {
    return "";
  }
  int startIndex = data.indexOf(": ", keyIndex);
  int stopIndex = data.indexOf("\r\n", keyIndex);

  return data.substring(startIndex + 2, stopIndex);
}

String getLineFromIndex(String data, int index) {
  String bufferData = data;
  for (int i = 0; i < index; i++) {
    bufferData = popLine(bufferData);
  }
  return getLine(bufferData);
}

String getContentType(String header) {
  String contentType = getHeaderField(header, "content-type");
  if (contentType == "") {
    contentType = getHeaderField(header, "Content-Type");
  }
  return contentType;
}

String readPayLoad(WiFiClient client, int payLoadSize) {
  String payload = String("");
  for (int i = 0; i < payLoadSize; i++) {
    char c = client.read();
    payload += String(c);
  }
  return payload;
}

int getPayLoadSize(String header) {
  String contentLength = getHeaderField(header, "content-length");
  if (contentLength == "") {
    contentLength = getHeaderField(header, "Content-Length");
  }
  return contentLength.toInt();
}

unsigned char h2int(char c) {
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

// public

String urldecode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == '+') {
      encodedString += ' ';
    } else if (c == '%') {
      i++;
      code0 = str.charAt(i);
      i++;
      code1 = str.charAt(i);
      c = (h2int(code0) << 4) | h2int(code1);
      encodedString += c;
    } else {
      encodedString += c;
    }

    yield();
  }

  return encodedString;
}

RequestParser::RequestParser(WiFiClient client) {
  // save connection
  _client = client;

  // clear data
  _header = String("");
  _payload = String("");
}

void RequestParser::addHeaderCharacter(char c) {
  _header += String(c);
}

String RequestParser::getHeader() {
  return _header;
}

void RequestParser::grabPayload() {
  // if (getContentType(_header) == "application/x-www-form-urlencoded") {
  _payload = readPayLoad(_client, getPayLoadSize(_header));
  //}
}

String RequestParser::getPayload() {
  return _payload;
}

String RequestParser::getField(String key) {
  int keyIndex = _payload.indexOf(key);

  if (keyIndex == -1) {
    return "";
  }

  int startIndex = _payload.indexOf("=", keyIndex);
  int stopIndex = _payload.indexOf("&", keyIndex);

  return urldecode(_payload.substring(startIndex + 1, stopIndex));
}
