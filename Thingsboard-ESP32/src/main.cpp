#include <WiFi.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>

// ————— Configuración WiFi —————
const char* WIFI_SSID     = "GONZALEZWIFI";
const char* WIFI_PASSWORD = "14830667";

// ————— Configuración ThingsBoard —————
const char* TB_HOST = "thingsboard.cloud";
const uint16_t TB_PORT = 1883;
const char* DEVICE_TOKEN = "5AIw0YvFDM87YF4sXaii";

// ————— Configuración Sensor Ultrasónico —————
#define TRIG_PIN 5
#define ECHO_PIN 18

WiFiClient netClient;
Arduino_MQTT_Client mqttClient(netClient);
ThingsBoard tb(mqttClient);

// ————— Funciones auxiliares —————
void connectWiFi() {
  Serial.print("Conectando a WiFi ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado, IP: " + WiFi.localIP().toString());
}

void connectThingsBoard() {
  if (!tb.connected()) {
    Serial.print("Conectando a ThingsBoard … ");
    bool ok = tb.connect(TB_HOST, DEVICE_TOKEN, TB_PORT);
    if (ok) {
      Serial.println("✓ Conectado");
    } else {
      Serial.println("✗ Error al conectar");
    }
  }
}

// ————— Función para medir distancia —————
float getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // cm
  return distance;
}

// ————— Setup —————
void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  connectWiFi();
  connectThingsBoard();
}

// ————— Loop —————
void loop() {
  connectThingsBoard();

  float distance = getDistanceCM();
  Serial.print("Distancia medida: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Enviar telemetría a ThingsBoard
  tb.sendTelemetryData("distance", distance);
  tb.sendTelemetryData("capacity", 20);
  tb.loop();
  delay(2000); // cada 2 segundos
}
