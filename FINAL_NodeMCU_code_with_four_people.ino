// #include <SimpleDHT.h>                   // Data ---> D3 VCC ---> 3V3 GND ---> GND
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <SoftwareSerial.h>
#include<string.h>
#include<math.h>
SoftwareSerial NodeMcu_SoftSerial (D5, D6); // rx , tx
char c;
String dataIn;
int vote = 0;
int cand1, cand2, cand3, cand4;
// WiFi parameters
#define WLAN_SSID       "Yekra"
#define WLAN_PASS       "yekrabd90"
// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "NomanEvm"
#define AIO_KEY         "aio_SlcI059V24YGcj188gKPmP0Rb6hs"
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish noman = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/noman");
Adafruit_MQTT_Publish yekra = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/yekra");
Adafruit_MQTT_Publish hridoy = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/hridoy");
Adafruit_MQTT_Publish mahadi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/mahadi");

void setup() {
  Serial.begin(19200);
  NodeMcu_SoftSerial.begin(9600);
  Serial.println(F("Adafruit IO Example"));
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();

}

// connect to adafruit io via MQTT
void connect() {
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if (ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(10000);
  }
  Serial.println(F("Adafruit IO Connected!"));
}

void loop() {

  while (NodeMcu_SoftSerial.available() > 0)
  {
    c = NodeMcu_SoftSerial.read();
    if ((c == 'A') | (c == 'B') | (c == 'C') | (c == 'D'))
    {
      break;
    }
    else
    {
      vote = vote * 10 + (c - 48);
      dataIn += c ;
    }
  }
  if (c == 'A')
  {
    cand1 = vote;
  }
  if (c == 'B')
  {
    cand2 = vote;
  }
  if (c == 'C')
  {
    cand3 = vote;
  }
  if (c == 'D')
  {
    cand4 = vote;
  }
  delay(5000);
  if (! mqtt.ping(3)) {
    if (! mqtt.connected())
      connect();
  }
  delay(5000);
  Serial.print(" Noman : "); Serial.println(int(cand1));
  if (! noman.publish(cand1)) {                     //Publish to Adafruit
    Serial.println(F("Failed"));
  }
  else {
    Serial.println(F("Sent!"));
  }

  delay(5000);
  Serial.print(" Yekra : "); Serial.println(int(cand2));

 if (! yekra.publish(cand2)) {                     //Publish to Adafruit
    Serial.println(F("Failed"));
  }
  else {
    Serial.println(F("Sent!"));
  }

  delay(5000);
  Serial.print(" Hridoy : "); Serial.println(int(cand3));
  if (! hridoy.publish(cand3)) {                     //Publish to Adafruit
    Serial.println(F("Failed"));
  }
  else {
    Serial.println(F("Sent!"));
  }
  delay(5000);

  Serial.print(" Mahadi : "); Serial.println(int(cand4));
  if (! mahadi.publish(cand4)) {                     //Publish to Adafruit
    Serial.println(F("Failed"));
  }
  else {
    Serial.println(F("Sent!"));
  }
  delay(5000);
  c = 0;
  dataIn = "";
  vote = 0;
  delay(1000);

}

// ping adafruit io a few times to make sure we remain connected
