#ifndef __ECOMQTT__
#define __ECOMQTT__
#include <PubSubClient.h>
#define MQTTSERVER "broker.hivemq.com"
#define PORT 1883
WiFiClient espClient;
bool ignoreMessage = true;
PubSubClient client(espClient);

// Callback aqui se reciben las peticiones de otros servicios
void callback(char *topic, byte *payload, unsigned int length) {

  // if (ignoreMessage) {
  //   ignoreMessage = false;
  //   return;
  // }

  Serial.print("Message received on topic: ");
  Serial.println(topic);

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message content: ");
  Serial.println(message);



  

  // If the message is "ON", turn on the LED; if it's "OFF", turn it off
  if (message.equals("lightON")) {
    digitalWrite(lightPin, HIGH);
  } else if (message.equals("lightOFF")) {
    digitalWrite(lightPin, LOW);
  } else if (message.equals("fanON")) {
    digitalWrite(fanPin, HIGH);
  } else if (message.equals("fanOFF")) {
    digitalWrite(fanPin, LOW);
  } else if (message.equals("automaticEnable")) {
    isAutomatic = true;
  } else if (message.equals("automaticDisabled")) {
    isAutomatic = false;
  } else {
    Serial.println("Message not found");
  }
}

// Conexion a servidor MQTT
void connectedMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("ecoTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
    }
  }
}

// Inicializador de mqtt
void setupMQTT() {
  client.setServer(MQTTSERVER, PORT);
  client.setCallback(callback);
}

#endif
