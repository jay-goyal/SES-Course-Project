#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

#define BUFFER_SIZE 50

const char* ssid = "JayPhone";
const char* password = "test0123";
const char* mqtt_server = "192.168.106.132";

// Op TData,HData
// D 32.32,32.32
// S 32.32,32.32
// 2S 32.32,32.3
// .32S 32.32,32

float tempVal = 0.00;
float humVal = 0.00;
float tempTgt = 22.00;
float humTgt = 10.00;

WiFiClient espClient;
PubSubClient client(espClient);

char msg[BUFFER_SIZE];

void wifi_connect() {
  delay(100);
  Serial.swap();
  delay(200);
  Serial.println("");
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(200);
  Serial.swap();
  delay(200);
}

void reconnect() {
  delay(200);
  Serial.swap();
  delay(200);
  Serial.print("Attempting MQTT Connection...");
  while (!client.connected()) {
    Serial.print(".");
    String client_id = "Group4Client-";
    client_id += String(random(0xffff), HEX);
    if (client.connect(client_id.c_str())) {
      Serial.println("");
      Serial.println("MQTT Broker Connected");
      client.subscribe("g4/sensor_data");
      client.subscribe("g4/target_data");
    } else {
      delay(5000);
    }
  }
  delay(200);
  Serial.swap();
  delay(200);
}

void callback(char* topic, byte* payload, unsigned int length) {
  digitalWrite(D4, !digitalRead(D4));
  String topicStr = String(topic);
  for (int i = 0; i < length; i++) msg[i] = (char)payload[i];
  // Serial.println(String(msg));
  if (topicStr.equals("g4/sensor_data")) {
    char* temp = strtok(msg, ",");
    tempVal = atof(temp);
    temp = strtok(NULL, ",");
    humVal = atof(temp);
  } else if (topicStr.equals("g4/target_data")) {
    char* temp = strtok(msg, ",");
    tempTgt = atof(temp);
    temp = strtok(NULL, ",");
    humTgt = atof(temp);
  } else {
    return;
  }
  uint8_t val = 0;
  if (tempVal > tempTgt) val += 1;
  else if (tempVal < tempTgt) val += 2;
  if (humVal > humTgt) val += 4;
  else if (humVal < humTgt) val += 8;
  Serial.swap();
  delay(200);
  Serial.println(tempVal);
  Serial.println(tempTgt);
  Serial.println(humVal);
  Serial.println(humTgt);
  Serial.println(val);
  delay(200);
  Serial.swap();
  delay(200);
  Serial.flush();
  Serial.write((char)val);
  // if (topicStr.equals("g4/sensor_data")) msg[0] = 'D';
  // else if (topicStr.equals("g4/target_data")) msg[0] = 'S';
  // msg[1] = ' ';
  // for (int i = 0; i < length; i++) msg[i+2] = (char)payload[i];
  // Serial.print(msg);
  // delay(200);
  // Serial.swap();
  // delay(200);
  // Serial.print(strlen(msg));
  // Serial.println(msg);
  // delay(200);
  // Serial.swap();
  // delay(200);
  // bzero(msg, BUFFER_SIZE);
}

void setup() {
  Serial.begin(115200);
  Serial.swap();
  delay(200);
  randomSeed(micros());
  pinMode(D4, OUTPUT);
  client.setCallback(callback);
  client.setServer(mqtt_server, 1883);
  wifi_connect();
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    wifi_connect();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
