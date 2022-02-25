#include "AutoOTA.h"

AutoOTA::AutoOTA(const char* rootCA = nullptr) {
  _client.setTimeout(10000);
  if (rootCA)
    _client.setCACert(rootCA);
  else
    _client.setInsecure();
}

void AutoOTA::_request(String uri) {
  _client.println("GET " + uri + " HTTP/1.1");
  _client.println("Host: " + _host);
  _client.println("Cache-Control: no-cache");
  _client.println("Connection: keepalive");
  _client.println();
}

int AutoOTA::_parseHeader(String header, String unwantedPart) {
  header.replace(unwantedPart, "");
  header.trim();
  return header.toInt();
}

int AutoOTA::begin(String host, int port) {
  _host = host;
  _port = port;
  Serial.println("Connecting to server... ");
  return _client.connect(_host.c_str(), (const int) _port);
}

int AutoOTA::available(String latestVersion, String currentVersion) {

  _request(latestVersion);
  // parse http response headers to get httpCode
  while (_client.connected()) {
    String line = _client.readStringUntil('\n');
    if (line.startsWith("HTTP/1.1")) {
      int httpCode = _parseHeader(line, "HTTP/1.1");
      if (httpCode != 200) {
        return httpCode;
      }
    }
    if (line == "\r") break;  // end of headers
    delay(1);
  }

  // get the payload (i.e. the firmware version number)
  String payload;
  while (_client.available()) {
    payload = _client.readStringUntil('\n');
  }
  payload.trim();
  Serial.printf("Latest Version on Server %s\n", payload.c_str());

  if (payload == currentVersion) return 0;

  return 1;

}

void AutoOTA::update(String sketchBinaryFile, int ledPin) {

  _request(sketchBinaryFile);

  // parse http response headers to get content-length
  int contentLength = 0;
  while (_client.connected()) {
    String line = _client.readStringUntil('\n');
    if (line.startsWith("Content-Length:")) {
      contentLength = _parseHeader(line, "Content-Length:");
    }
    if (line == "\r") break;  // end of headers
    delay(1);
  }

  // Check if there is enough memory to OTA Update
  bool enoughSpace;
  if (ledPin) {
    enoughSpace = Update.begin(contentLength, U_FLASH, ledPin, HIGH);
  }
  else {
    enoughSpace = Update.begin(contentLength);
  }
  if(!enoughSpace) {
      Serial.printf("Not enough space, require %d bytes\n", contentLength);
      _client.flush();
      return;
  }

  // Update begin, and may take a few minutes and no output appear on Serial Monitor
  Serial.print("Begin OTA...");
  Update.writeStream(_client);

  if (Update.end()) {
    if (Update.isFinished()) {
      Serial.println("Update completed.");
    } else {
      Serial.println("Update failed.");
    }
  } else {
    Serial.println("Update Error: " + String(Update.getError()));
  }

}
