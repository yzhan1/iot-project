#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <SPI.h>
#include <TimeLib.h>

#include <ESP8266HTTPClient.h>

#define HTTP_DEBUG

//////////////////////
// WiFi Definitions //
//////////////////////
//const char WiFiSSID[] = "peteriphone";
//const char WiFiPSK[] = "12345678";

const char WiFiSSID[] = "FREEHOLD";
const char WiFiPSK[] = "wangmomo";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 6; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

const int OUTPUT_PIN = 4;
const int READ_PIN = 5;

int state = 0; // closed

String server_addr = "192.168.0.111";
uint16_t server_port = 3000;

WiFiServer server(80);

void loop() {
  int input = digitalRead(READ_PIN);
  int doorClosed = -1;
  String path = "";
  WiFiClient client = server.available();
  
  if (client) {
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();
    
    if (req.indexOf("/open") != -1) {
      doorClosed = 0;
      path = "enter";
      // notify arduino
      digitalWrite(OUTPUT_PIN, HIGH);
      delay(2000);
      digitalWrite(OUTPUT_PIN, LOW);
  
      Serial.println("OPENED");
      
    } else if (req.indexOf("/close") != -1) {
      doorClosed = 1;
      path = "leave";
      // notify arduino
      digitalWrite(OUTPUT_PIN, HIGH);
      delay(2000);
      digitalWrite(OUTPUT_PIN, LOW);
  
      Serial.println("CLOSED");
    }

    client.flush();
    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: text/html\r\n\r\n";
    s += "<!DOCTYPE HTML>\r\n<html>\r\n";
    if (doorClosed >= 0) {
      s += "Door is ";
      s += doorClosed == 1 ? "closed" : "opened";
    } else {
      s += "Invalid Request.<br> Try /open, or /close.";
    }
    s += "</html>\n";
  
    // Send the response to the client
    client.print(s);
    delay(1);
    Serial.println("Client disonnected");
  }
  
  // Always push result to server
  HTTPClient httpclient;
  WiFiClient client2;
  
  
  if (state != input) {
    Serial.println("changed state");
    state = input;
    if(state) {
      //closed
      path = "/enter";
    } else {
      path = "/leave";
    }
    int h = hour();
    String data = "ts=" + h;

    Serial.print("[HTTP] BEGIN POST...\n");

    if (httpclient.begin(client2,server_addr, server_port, path, false)) {  // HTTP


      Serial.print("[HTTP] POST...\n");
      // start connection and send HTTP header
      int httpCode = httpclient.POST(data);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          Serial.print("[HTTP] OK\n");
        } else {
          Serial.print("[HTTP] Code:");
          Serial.println(httpCode);
          
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", httpclient.errorToString(httpCode).c_str());
      }

      httpclient.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  delay(1);
}

void setup() {
  initHardware();
  connectWiFi();
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW);
  pinMode(READ_PIN, INPUT);
  server.begin();
  setupMDNS();
}

void connectWiFi() {
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);
  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED) {
    // Blink the LED
    //digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    //ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMDNS() {
  // Call MDNS.begin(<domain>) to set up mDNS to point to
  // "<domain>.local"
  if (!MDNS.begin("thing")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) delay(1000);
  }
  Serial.println("mDNS responder started");
}

void initHardware() {
  Serial.begin(9600);
  pinMode(DIGITAL_PIN, INPUT_PULLUP);
}
