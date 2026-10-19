#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <HTTPSServer.hpp>
#include <HTTPServer.hpp>
#include <SSLCert.hpp>

using namespace httpsserver;

const char* AP_SSID = "PS5-Relapse";
const char* AP_PASS = "relapse123";

WebServer server(80);
DNSServer dnsServer;
HTTPSServer* secureServer = nullptr;

static String contentType(const String& path) {
  if (path.endsWith(".html")) return "text/html";
  if (path.endsWith(".js"))   return "application/javascript";
  if (path.endsWith(".css"))  return "text/css";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".elf"))  return "application/octet-stream";
  if (path.endsWith(".bin"))  return "application/octet-stream";
  if (path.endsWith(".png"))  return "image/png";
  if (path.endsWith(".ico"))  return "image/x-icon";
  return "application/octet-stream";
}

static void handleCaptivePortal() {
  server.sendHeader("Location", "http://192.168.0.1/", true);
  server.send(302, "text/html", "");
}

static String htmlEscape(const String& s) {
  String r;
  r.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char ch = s[i];
    if (ch == '&') r += "&amp;";
    else if (ch == '<') r += "&lt;";
    else if (ch == '>') r += "&gt;";
    else if (ch == '"') r += "&quot;";
    else r += ch;
  }
  return r;
}

static void handleLoadPayloads() {
  IPAddress ps5 = server.client().remoteIP();
  File f = LittleFS.open("/pl/usb-payload-browser.elf", "r");
  if (!f) {
    server.send(500, "text/plain", "payload browser not found");
    return;
  }

  WiFiClient c;
  if (!c.connect(ps5, 9021, 3000)) {
    f.close();
    server.send(500, "text/html",
      "<html><head><meta charset=\"utf-8\"></head>"
      "<body style=\"background:#060606;color:#f66;font-family:sans-serif;padding:24px\">"
      "<p>Could not reach elfldr on the PS5.</p><p>Run the exploit first.</p>"
      "<p><a href=\"/\" style=\"color:#0ff\">Back to menu</a></p></body></html>");
    return;
  }

  uint8_t buf[1024];
  while (f.available()) {
    size_t n = f.read(buf, sizeof(buf));
    if (n) c.write(buf, n);
  }
  c.flush();
  f.close();
  c.stop();

  // Wait for the payload to start and bind its HTTP port.
  delay(1500);

  String base = "http://" + ps5.toString() + ":9090";
  String json;

  for (int attempt = 0; attempt < 3; attempt++) {
    HTTPClient http;
    http.setTimeout(3000);
    http.begin(base + "/api/list");
    int code = http.GET();
    if (code == 200) {
      json = http.getString();
      http.end();
      break;
    }
    http.end();
    delay(800);
  }

  String html = String(
    "<html><head><meta charset=\"utf-8\">"
    "<title>USB Payloads</title><style>"
    "body{background:#060606;color:#fff;font-family:sans-serif;margin:0;padding:16px}"
    "h1{font-size:20px;border-bottom:1px solid #333;padding-bottom:8px}"
    "a{display:block;padding:10px 8px;margin:4px 0;background:#111;color:#0ff;text-decoration:none;border-radius:4px}"
    "a.menu{background:#222}"
    "</style></head><body><h1>USB Payloads</h1>");

  if (json.length() == 0) {
    html += String("<p style=\"color:#f66\">Could not reach the payload browser on the PS5.</p>");
  } else {
    int count = 0;
    int pos = 0;
    while ((pos = json.indexOf("{\"name\":\"", pos)) >= 0) {
      pos += strlen("{\"name\":\"");
      int nameEnd = json.indexOf('"', pos);
      if (nameEnd < 0) break;
      String name = json.substring(pos, nameEnd);

      int urlStart = json.indexOf("\"url\":\"", nameEnd);
      if (urlStart < 0) break;
      urlStart += strlen("\"url\":\"");
      int urlEnd = json.indexOf('"', urlStart);
      if (urlEnd < 0) break;
      String url = json.substring(urlStart, urlEnd);

      html += String("<a href=\"") + base + url + "\">" + htmlEscape(name) + "</a>";
      count++;
      pos = urlEnd;
    }
    if (count == 0) {
      html += String("<p style=\"color:#777\">No .elf / .bin files found on USB.</p>");
    }
  }

  html += String("<a class=\"menu\" href=\"/\">Back to menu</a></body></html>");
  server.send(200, "text/html", html);
}

static void handleRequest() {
  String path = server.uri();

  if (path.indexOf("..") >= 0) {
    server.send(403, "text/plain", "Forbidden");
    return;
  }

  if (path == "/load-payloads") {
    handleLoadPayloads();
    return;
  }

  if (path == "/") path = "/index.html";

  if (!LittleFS.exists(path)) {
    handleCaptivePortal();
    return;
  }

  File f = LittleFS.open(path, "r");
  if (!f) {
    server.send(500, "text/plain", "Failed to open: " + path);
    return;
  }

  server.sendHeader("Cache-Control", "no-store");
  server.streamFile(f, contentType(path));
  f.close();
}

static void handleHTTPS(HTTPRequest* req, HTTPResponse* res) {
  String path = String(req->getRequestString().c_str());

  if (path.indexOf("/update/") >= 0) {
    res->setStatusCode(200);
    res->setStatusText("OK");
    res->setHeader("Content-Type", "application/xml");
    res->println("<?xml version=\"1.0\" ?><update_data_list><region id=\"us\"><force_update><system auto_update_version=\"00.00\" sdk_version=\"01.00.00.09-00.00.00.0.0\" upd_version=\"01.00.00.00\"/></force_update><system_pup auto_update_version=\"00.00\" label=\"20.02.02.20.00.07-00.00.00.0.0\" sdk_version=\"02.20.00.07-00.00.00.0.0\" upd_version=\"02.20.00.00\"><update_data update_type=\"full\"></update_data></system_pup></region></update_data_list>");
  } else {
    res->setStatusCode(301);
    res->setStatusText("Moved Permanently");
    res->setHeader("Location", "http://192.168.0.1/");
    res->println("Moved Permanently");
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  if (!LittleFS.begin(true)) {
    Serial.println("[FATAL] LittleFS mount failed");
    return;
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  WiFi.softAPConfig(IPAddress(192, 168, 0, 1),
                    IPAddress(192, 168, 0, 1),
                    IPAddress(255, 255, 255, 0));

  dnsServer.start(53, "*", WiFi.softAPIP());

  Serial.println();
  Serial.print("SSID: ");
  Serial.println(AP_SSID);
  Serial.print("URL:  http://");
  Serial.println(WiFi.softAPIP().toString());

  server.onNotFound(handleRequest);
  server.begin();
  Serial.println("HTTP server on port 80");

  SSLCert* cert = nullptr;
  if (LittleFS.exists("/cert.der") && LittleFS.exists("/pk.pem")) {
    File cf = LittleFS.open("/cert.der", "r");
    unsigned int clen = cf.size();
    uint8_t* cbuf = (uint8_t*)malloc(clen);
    cf.read(cbuf, clen);
    cf.close();
    File pf = LittleFS.open("/pk.pem", "r");
    unsigned int klen = pf.size();
    uint8_t* kbuf = (uint8_t*)malloc(klen);
    pf.read(kbuf, klen);
    pf.close();
    cert = new SSLCert(cbuf, clen, kbuf, klen);
    Serial.println("Loaded stored certificate");
  } else {
    Serial.println("Generating self-signed certificate...");
    cert = new SSLCert();
    int certResult = createSelfSignedCert(*cert, KEYSIZE_1024, "CN=PS5-Relapse,O=ESP32,C=US");
    if (certResult != 0) {
      Serial.printf("Certificate generation failed (0x%02X)\n", certResult);
    } else {
      File pf = LittleFS.open("/pk.pem", "w");
      pf.write(cert->getPKData(), cert->getPKLength());
      pf.close();
      File cf = LittleFS.open("/cert.der", "w");
      cf.write(cert->getCertData(), cert->getCertLength());
      cf.close();
      Serial.println("Certificate stored");
    }
  }

  if (cert) {
    secureServer = new HTTPSServer(cert);
    ResourceNode* nodeHTTPS = new ResourceNode("", "ANY", &handleHTTPS);
    secureServer->setDefaultNode(nodeHTTPS);
    secureServer->start();
    if (secureServer->isRunning()) {
      Serial.println("HTTPS server on port 443");
    } else {
      Serial.println("HTTPS server failed to start");
    }
  }

  Serial.println("DNS captive portal on port 53");
}

void loop() {
  dnsServer.processNextRequest();
  if (secureServer) secureServer->loop();
  server.handleClient();
}
