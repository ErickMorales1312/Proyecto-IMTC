#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Configuración de los pines para el módulo GPS
const int RXPin = 4; // Pin de recepción (RX) del GPS
const int TXPin = 3; // Pin de transmisión (TX) del GPS
const uint32_t GPSBaud = 9600; // Velocidad en baudios del GPS

// Instancia de TinyGPS++
TinyGPSPlus gps;

// Instancia de SoftwareSerial para la comunicación con el GPS
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Serial.println("GPS Inicializado");
}

void loop() {
  // Leer datos del GPS
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Si se ha recibido una nueva posición
  if (gps.location.isUpdated()) {
    Serial.print("Latitud: ");
    Serial.println(gps.location.lat(), 6); // Imprimir latitud con 6 decimales
    Serial.print("Longitud: ");
    Serial.println(gps.location.lng(), 6); // Imprimir longitud con 6 decimales
  }

  delay(1000); // Esperar 1 segundo antes de la siguiente lectura
}
