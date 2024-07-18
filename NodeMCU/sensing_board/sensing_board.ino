#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define BUFFER_SIZE 50

const char* ssid = "JayPhone";
const char* password = "test0123";
const char* mqtt_server = "192.168.106.132";

WiFiClient espClient;
PubSubClient client(espClient);
char rx_msg[BUFFER_SIZE];
uint ridx = 0;

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
      // client.subscribe("g4/led");
    } else {
      delay(5000);
    }
  }
  delay(200);
  Serial.swap();
  delay(200);
  Serial.write('T');
}

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.swap();
//   delay(200);
//   Serial.print("Message arrived from [");
//   Serial.print(topic);
//   Serial.print("]: ");
//   for (int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]);
//   }
//   Serial.println();
//   delay(200);
//   Serial.swap();
//   delay(200);

//   if ((char)payload[0] == '1') {
//     digitalWrite(D4, LOW);
//     Serial.write('1');
//   } else {
//     digitalWrite(D4, HIGH);
//     Serial.write('0');
//   }
// }

void setup() {
  Serial.begin(115200);
  Serial.swap();
  delay(200);
  randomSeed(micros());
  pinMode(D4, OUTPUT);
  client.setServer(mqtt_server, 1883);
  wifi_connect();
  digitalWrite(D4, HIGH);
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.write('F');
    wifi_connect();
  }
  if (!client.connected()) {
    Serial.write('F');
    reconnect();
  }
  client.loop();
  if (Serial.available() > 0) {
    byte currByte = Serial.read();
    if (ridx >= BUFFER_SIZE) {
      ridx = 0;    }
    if (currByte == '\n') {
      digitalWrite(D4, !digitalRead(D4));
      client.publish("g4/sensor_data", rx_msg);
      Serial.swap();
      delay(200);
      String str = String(rx_msg);
      Serial.println(str);
      delay(200);
      Serial.swap();
      bzero(rx_msg, BUFFER_SIZE);
      ridx = 0;
    } else if (currByte >= 32 && currByte <= 126) {
      rx_msg[ridx++] = currByte;
    }
  }
}
