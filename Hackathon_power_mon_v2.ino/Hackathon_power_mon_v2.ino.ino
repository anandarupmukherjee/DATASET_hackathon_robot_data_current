#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1; 



// Replace the next variables with your SSID/Password combination
const char * ssid = "UniOfCam-IoT";
const char * password = "p3YcKrA6";


// Add your MQTT Broker IP address, example:
const char* mqtt_server = "10.252.21.118";

char out[256];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
IPAddress ip; 



void setup() {
  Serial.begin(115200);
  emon1.current(32, 111.1); // GPIO 32 on the ESP32 pin is equivalent to the ADC1, channel 4 pin. 
  pinMode(2,OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}


void setup_wifi() {
  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Serial.print("WiFi connected");
 Serial.print("IP address: ");
 Serial.print(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected

  while (!client.connected()) {
   Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Anand_M5_Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } 
    else {
     Serial.println("failed, rc=");
     Serial.println(client.state());
     delay(5000);
    }
  }
}
 

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);

  client.loop();

  long now = millis();
  if (now - lastMsg > 100) {
    lastMsg = now;

   
   String line;
   double Irms = emon1.calcIrms(1480);
   line = String(Irms*230);
   Serial.println(Irms*230);
   ip = WiFi.localIP();
   String doc ="{IP:"+ip.toString() +", "+ String(line)+"}";

   doc.toCharArray(out, doc.length()+1);
   Serial.println(out);
   client.publish("hack/robot/status",out);
  }

  
}
