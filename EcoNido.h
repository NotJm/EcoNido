#include "esp32-hal-gpio.h"
#ifndef __ECONIDO__
#define __ECONIDO__
#include <DHT.h>
#include <DHT_U.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Ticker.h>
#include <time.h>
#include "MQ135.h"
// Libraries

Ticker tickerLed;              // Ticker led
WiFiManager wifiManager;       // WiFi Manager
const uint8_t ledWifiPin = 2;  // Led WiFi Pin

void connectToWifi();
void signalLed();
// Prototype

// PINES
const uint8_t dhtPin = 15;    // Sensor DHT 22
const uint8_t mqPin = 32;     // Sensor mq-135
const uint8_t fanPin = 14;    // Ventilador
const uint8_t motorPin = 13;  // Motor
const uint8_t lightPin = 27;  // Foco incadecente

class EcoNido {
private:
  // const uint8_t ledWifiPin = 2;  // Led parpadeante

  // Variables
  const char* device_name = "ECONIDO-JLRY";
  const float temperatureThreshold = 38.3;
  float temperature, humidity, averageTemperature, medianHumidity;  // Temperatura, humedaad
  String qualityAir = "";                                           // Calidad del aire
  int mq;                                                           // Calculo de la calidad de aire
  float temperatures[10];                                           // Arreglo que almacenara la temperatura
  float humidities[10];                                             // Arreglo que almacenara la humedades
  int currentIndex = 0;
  // Servidor de mongoDB
  const char* serveName = "https://ecoserver-zopz.onrender.com/insertDevice";

  // Instancias
  DHT dht;      // Instancia del sensor DHT22
  MQ135 mq135;  // Instancia de la libreria MQ para calculo de calidad de aire
  HTTPClient http;
  float calculateAverage(float arr[], int length);
  float calculateMedian(float arr[], int length);
public:
  EcoNido();
  void readQualityAir();        // Aqui se lee la calidad de aire
  void readDHT();               // Aqui se lee la humedad y temperatura
  void sendHTTPPostResquest();  // Aqui se envia datos a hostinger
};

// Aqui se inicializa las instancias de los objetos
EcoNido::EcoNido()
  : dht(dhtPin, DHT11), mq135(mqPin) {
  Serial.begin(115200);  // inicalizamos el serial en 115200
  dht.begin();           // lo ponemos a chambear :3
  delay(2000);
  analogReadResolution(12);   // activamos esto para el mq-136
  pinMode(fanPin, OUTPUT);    // inicializamos en modo salida
  pinMode(lightPin, OUTPUT);  // inicializamos en modo salida
}



// Aqui se lee la humedad y temperatura
void EcoNido::readDHT() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  // Serial.println("temp=" + String(temperature) + "hum=" + String(humidity));
  // Si la temperatura es menor a 37 grados
  // entonces el foco se prende en dado caso
  // que el ventilador este prendido se apaga

  temperatures[currentIndex] = temperature;
  humidities[currentIndex] = humidity;
  currentIndex = (currentIndex + 1) % 10;

  averageTemperature = calculateAverage(temperatures, 10);
  medianHumidity = calculateMedian(humidities, 10);

  if (isAutomatic) {
    if (averageTemperature <= 36) {
      digitalWrite(lightPin, HIGH);  // Encender el foco
      digitalWrite(fanPin, LOW);     // Apagar el ventilador
    } else if (averageTemperature >= 37) {
      digitalWrite(lightPin, LOW);  // Apagar el foco
      digitalWrite(fanPin, HIGH);   // Encender el ventilador
    } else {
      // No hace nada ....
    }
  }
}

// Aqui se lee la calidad de aire
/*
  Si mq >= 350 y mq < 500 entonces la calidad de aire es Excelente
  Si mq >= 500 y mq < 800 entonces la calidad de aire es moderada
  Si mq >= 800 y mq < 1200 entonces la calidad de aire es Baja
  Si mq >= 1200 entonces la calidad de aires Mala
*/
void EcoNido::readQualityAir() {
  mq = mq135.getCorrectedPPM(averageTemperature, medianHumidity);
  // Serial.println("mq=" + String(mq));
  if (mq >= 0 && mq < 500) {
    qualityAir = "Excelente";
  } else if (mq >= 500 && mq < 800) {
    qualityAir = "Moderada";
  } else if (mq >= 800 && mq < 1200) {
    qualityAir = "Baja";
  } else {
    qualityAir = "Mala";
  }
}

// Enviar datos a MongoDB
void EcoNido::sendHTTPPostResquest() {

  // Obtener la fecha y la hora actuales
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);

  // Convertir la fecha y la hora a una cadena con el formato deseado
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d", timeinfo);

  bool lightState = digitalRead(lightPin);
  bool fanState = digitalRead(fanPin);

  // Construir los datos para enviar en la solicitud HTTP
    String data = "device=" + String(device_name) +
                  "&mac=" + String(WiFi.macAddress()) +
                  "&timestamp=" + String(timestamp) +
                  "&temperatura=" + String(averageTemperature) +
                  "&humedad=" + String(medianHumidity) +
                  "&quality=" + String(qualityAir) +
                  "&mq=" + String(mq) +
                  "&light=" + String(lightState) +
                  "&fan=" + String(fanState) +
                  "&automatic=" + String(isAutomatic);

            
  if (http.begin(serveName)) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(data);

    http.end();
  } else {
    Serial.println("Error al establecer la conexión HTTP");
  }
}

float EcoNido::calculateAverage(float arr[], int length) {
  float sum = 0;
  for (int i = 0; i < length; i++) {
    sum += arr[i];
  }
  return sum / length;
}

float EcoNido::calculateMedian(float arr[], int length) {
  // Ordenar el arreglo
  for (int i = 0; i < length - 1; i++) {
    for (int j = 0; j < length - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        float temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }

  // Calcular la mediana
  if (length % 2 == 0) {
    return (arr[length / 2 - 1] + arr[length / 2]) / 2.0;
  } else {
    return arr[length / 2];
  }
}



// Aqui es para conectar a la red de WifiManager
void connectToWifi() {
  tickerLed.attach(0.2, signalLed);
  if (!wifiManager.autoConnect("EcoNido")) {
    ESP.restart();
  }
  tickerLed.detach();
  digitalWrite(ledWifiPin, LOW);
}

// Aqui para la señal del led este para cuando no se conecte la red
void signalLed() {
  int stateLed = digitalRead(ledWifiPin);
  digitalWrite(ledWifiPin, !stateLed);
}


#endif  // __ECONIDO__