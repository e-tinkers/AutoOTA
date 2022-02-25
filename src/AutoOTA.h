#ifndef _AUTOOTA_H
#define _AUTOOTA_H
#include <WiFiClientSecure.h>
#include <Update.h>

class AutoOTA
{
public:
  AutoOTA(const char * rootCA);
  void _request(String uri);
  int _parseHeader(String header, String unwantedPart);
  int begin(String host, int port);
  int available(String sketchVersionFile, String currentVersion);
  void update(String sketchBinaryFile, int ledPin = 0);
private:
  WiFiClientSecure _client;
  String _host;
  int _port;
};
#endif
