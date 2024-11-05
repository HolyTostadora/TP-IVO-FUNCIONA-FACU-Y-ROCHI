#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

#define DHTPIN 27     // Pin donde está conectado el sensor DHT
#define DHTTYPE DHT11   // Tipo de sensor (puedes usar DHT22 si tienes uno)

DHT dht(DHTPIN, DHTTYPE);

// Pines de los LEDs
const int led1 = 33;
const int led2 = 32;
const int led3 = 35;
const int led4 = 34;
const int led5 = 25;

// Pines de los botones
const int botonIncremento = 22;
const int botonDecremento = 23;

// Variables para los límites de los rangos de humedad
float rangoMin = 50;
float rangoMax = 80;

// Configuración Wi-Fi
const char* ssid = "TuSSID";          // Cambia esto por tu SSID
const char* password = "TuPassword";   // Cambia esto por tu contraseña

WebServer server(80); // Servidor web en el puerto 80

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Configurar los pines de los LEDs como salida
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

  // Configurar los pines de los botones como entrada
  pinMode(botonIncremento, INPUT_PULLUP);
  pinMode(botonDecremento, INPUT_PULLUP);

  // Conectar a Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a Wi-Fi");

  // Imprimir la dirección IP
  Serial.print("Dirección IP del ESP32: ");
  Serial.println(WiFi.localIP());

  // Configurar manejador para la raíz "/"
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  float humidity = dht.readHumidity();  // Leer la humedad en porcentaje
  
  // Verificar si el sensor está funcionando correctamente
  if (isnan(humidity)) {
    Serial.println("Error leyendo la humedad!");
    return;
  }

  // Verificar los botones para ajustar los rangos
  if (digitalRead(botonIncremento) == LOW) {
    rangoMin += 5;
    rangoMax += 5;
    delay(200);  // Para evitar múltiples incrementos rápidos
  }

  if (digitalRead(botonDecremento) == LOW) {
    rangoMin -= 5;
    rangoMax -= 5;
    delay(200);  // Para evitar múltiples decrementos rápidos
  }

  Serial.print("Humedad: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Rango Min: ");
  Serial.print(rangoMin);
  Serial.print(" % - ");
  Serial.print("Rango Max: ");
  Serial.println(rangoMax);

  // Apagar todos los LEDs antes de encender uno nuevo
  apagarTodosLosLeds();

  // Ajustar los LEDs según los nuevos rangos
  if (humidity < rangoMin) {
    digitalWrite(led1, HIGH);  
  } else if (humidity >= rangoMin && humidity < (rangoMin + 10)) {
    digitalWrite(led2, HIGH);  
  } else if (humidity >= (rangoMin + 10) && humidity < (rangoMin + 20)) {
    digitalWrite(led3, HIGH);  
  } else if (humidity >= (rangoMin + 20) && humidity < rangoMax) {
    digitalWrite(led4, HIGH);  
  } else {
    digitalWrite(led5, HIGH);  
  }

  // Manejar las solicitudes del cliente web
  server.handleClient();
  delay(2000);  // Esperar 2 segundos antes de la próxima lectura
}

void apagarTodosLosLeds() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
}

void handleRoot() {
  float humidity = dht.readHumidity();  // Leer la humedad para mostrar en la página

  String html = "<html>\
                 <head><title>Estado de los LEDs</title></head>\
                 <body>\
                 <h1>Estado de los LEDs basado en la humedad</h1>\
                 <p>Humedad actual: " + String(humidity) + "%</p>\
                 <p>Rango Min: " + String(rangoMin) + "% - Rango Max: " + String(rangoMax) + "%</p>\
                 <p>Estado de los LEDs:</p>\
                 <ul>";
  html += "<li>LED 1: " + String(digitalRead(led1) == HIGH ? "Encendido" : "Apagado") + "</li>";
  html += "<li>LED 2: " + String(digitalRead(led2) == HIGH ? "Encendido" : "Apagado") + "</li>";
  html += "<li>LED 3: " + String(digitalRead(led3) == HIGH ? "Encendido" : "Apagado") + "</li>";
  html += "<li>LED 4: " + String(digitalRead(led4) == HIGH ? "Encendido" : "Apagado") + "</li>";
  html += "<li>LED 5: " + String(digitalRead(led5) == HIGH ? "Encendido" : "Apagado") + "</li>";
  html += "</ul></body></html>";

  server.send(200, "text/html", html);
}
