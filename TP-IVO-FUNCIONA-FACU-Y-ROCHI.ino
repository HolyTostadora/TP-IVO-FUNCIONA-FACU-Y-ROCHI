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

void setup() {
  Serial.begin(9600);
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

  // Ajustar los LEDS según los nuevos rangos
  if (humidity < rangoMin) {
    digitalWrite(led1, HIGH);  // Menos de rangoMin enciende el LED 1
  } else if (humidity >= rangoMin && humidity < (rangoMin + 10)) {
    digitalWrite(led2, HIGH);  // Entre rangoMin y rangoMin + 10 enciende el LED 2
  } else if (humidity >= (rangoMin + 10) && humidity < (rangoMin + 20)) {
    digitalWrite(led3, HIGH);  // Entre rangoMin + 10 y rangoMin + 20 enciende el LED 3
  } else if (humidity >= (rangoMin + 20) && humidity < rangoMax) {
    digitalWrite(led4, HIGH);  // Entre rangoMin + 20 y rangoMax enciende el LED 4
  } else {
    digitalWrite(led5, HIGH);  // rangoMax o más enciende el LED 5
  }

  delay(2000);  // Esperar 2 segundos antes de la próxima lectura
}

void apagarTodosLosLeds() {
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
}
