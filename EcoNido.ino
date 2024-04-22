bool isAutomatic = true;
#include "EcoNido.h"
#include "EcoMqtt.h"
// #include "EcoM.h"
// Library Eco Nido
Ticker tickerDht, tickerMq, tickerSend, tickerMqtt;
EcoNido econido;
unsigned long previousMillis = 0;  // Variable para almacenar el tiempo del último ciclo
const long interval = 1000;       // Intervalo de 1 segundo en milisegundos
void setup() {
  configTime(-6 * 3600, 0, "pool.ntp.org");
  pinMode(ledWifiPin, OUTPUT);
  connectToWifi();
  setupMQTT();
  tickerDht.attach(1, updateDHT);
  tickerMq.attach(1, updateMq);
  // tickerSend.attach(15, updateSend);
}



void loop() {
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (!client.connected()) {
      connectedMQTT();
    }
    client.loop();
    econido.sendHTTPPostResquest();
  }
}

inline void updateDHT() {
  econido.readDHT();
}

inline void updateMq() {
  econido.readQualityAir();
}

inline void updateSend() {
  econido.sendHTTPPostResquest();
}



