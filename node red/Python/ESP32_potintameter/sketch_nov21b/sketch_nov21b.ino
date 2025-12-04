#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "Omar";
const char* password = "12345678";

const char* mqtt_server = "3091b71eb24541b4a5593fa28711f867.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "SaherHassaballa";
const char* mqtt_password = "Saher12345678";

WiFiClientSecure espClient;
PubSubClient client(espClient);

int potPowerPin = 26;   

bool potEnabled = false;

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Topic: ");
  Serial.println(topic);

  if (strcmp(topic, "pi/control") == 0) {

    int value = payload[0] - '0';

    Serial.print("Received value: ");
    Serial.println(value);

    if (value == 1) {
      potEnabled = true;
      digitalWrite(potPowerPin, HIGH);
    } else {
      potEnabled = false;
      digitalWrite(potPowerPin, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      client.subscribe("pi/control");   // ✅ نفس التوبيك بالظبط
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(potPowerPin, OUTPUT);
  digitalWrite(potPowerPin, LOW);

  pinMode(potReadPin, INPUT);

  setup_wifi();
  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (potEnabled) {
    int potValue = analogRead(potReadPin);
    float voltage = potValue * (3.3 / 4095.0);

    char msg[20];
    snprintf(msg, 20, "%.2f", voltage);

    client.publish("esp32/potentiometer", msg);
  }

  delay(200);
}
