#include <QMC5883LCompass.h>

QMC5883LCompass compass;

// Variables para calibración
int minX = 32767, maxX = -32768;
int minY = 32767, maxY = -32768;

void setup() {
  Serial.begin(115200);
  compass.init();
  Serial.println("Gira el sensor en todas direcciones durante 20-30 segundos...");
}

void loop() {
  compass.read();
  int x = compass.getX();
  int y = compass.getY();

  // Guardamos máximos y mínimos
  if (x < minX) minX = x;
  if (x > maxX) maxX = x;
  if (y < minY) minY = y;
  if (y > maxY) maxY = y;

  // Calculamos offset
  int offsetX = (maxX + minX) / 2;
  int offsetY = (maxY + minY) / 2;

  // Corregimos valores
  int xCorr = x - offsetX;
  int yCorr = y - offsetY;

  // Calculamos rumbo
  float heading = atan2(yCorr, xCorr) * 180.0 / PI;
  if (heading < 0) heading += 360.0;

  // Mostramos
  Serial.print("Rumbo (°): ");
  Serial.println(heading);

  delay(200);
}