#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <QMC5883LCompass.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "HTML.h"

// Variables
TinyGPSPlus gps;
QMC5883LCompass compass;
SoftwareSerial ss(RXPin, TXPin);
float heading = 0;
float bearing = 0;
float distance = 0; 
long duration;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool State2 = LOW;
bool State1 = LOW;
bool State0 = LOW;

// Definición de pines
#define USTrigPin 33
#define USEchoPin 32
#define EN1Pin 5
#define EN2Pin 15
#define IN1Pin 17
#define IN2Pin 16
#define IN3Pin 4
#define IN4Pin 2
const int RXPin = 4; // Pin de recepción (RX) del GPS
const int TXPin = 3; // Pin de transmisión (TX) del GPS

// Constantes
#define latVehiculo 25.697400
#define lonVehiculo -100.344024
#define latModulo 25.696691
#define lonModulo -100.343247
#define soundSpeed 0.034
#define declinacion 4.44 // Local declination in degrees

// Web server
const char* ssid = "OswaldoSanchez";
const char* password = "ESP32FIME";

IPAddress ip(192,168,251,100); 
IPAddress gateway(192,168,251,1); 
IPAddress subnet(255,255,255,0);

WebServer server(80);

// Declaración de funciones
void handle_OnConnect();
void handle_stop();
void handle_adelante();
void handle_atras();
void handle_izquierda();
void handle_derecha();
void handle_NotFound();
void sendResponse(bool state2, bool state1, bool state0);

// Funciones del sensor ultrasonico
float checkdistance();

// Funciones del motor
void setMotor(int in1, int in2, int en, bool direction, int velocidad);
void stopMotor(int in1, int in2, int en);
void controlMotor(int motor, bool direction, int velocidad);
void MotorStop(int motor);

// Funciones de la brújula
float calcularBearing(float lat1, float lon1, float lat2, float lon2);

void setup() {
  Serial.begin(115200);

  // Configuración del sensor ultrasonico
  pinMode(USEchoPin, INPUT);
  pinMode(USTrigPin, OUTPUT);

  // Configuración del motor L298N
  pinMode(EN1Pin, OUTPUT);
  pinMode(EN2Pin, OUTPUT);
  pinMode(IN1Pin, OUTPUT);
  pinMode(IN2Pin, OUTPUT);
  pinMode(IN3Pin, OUTPUT);
  pinMode(IN4Pin, OUTPUT);

  //Configuración del GPS
  ss.begin(GPSBaud);
  Serial.println("GPS Inicializado");

  // Configuración de la brújula
  Wire.begin();
  compass.init();
  compass.setSmoothing(10, true);

  bearing = calcularBearing(latVehiculo, lonVehiculo, latModulo, lonModulo);
  Serial.print("Bearing hacia el módulo: ");
  Serial.println(bearing);

  // Configuración de WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);

  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  Serial.println("Conectado a ");
  Serial.println(ssid);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/stop", handle_stop);
  server.on("/adelante", handle_adelante);
  server.on("/atras", handle_atras);
  server.on("/izquierda", handle_izquierda);
  server.on("/derecha", handle_derecha);
  server.onNotFound(handle_NotFound);

  server.begin();
}

void loop() {
   // Leer datos del GPS
  while (ss.available() > 0) { gps.encode(ss.read());}
  // Si se ha recibido una nueva posición
  if (gps.location.isUpdated()) {
    Serial.print("Latitud: ");
    Serial.println(gps.location.lat(), 6); // Imprimir latitud con 6 decimales
    Serial.print("Longitud: ");
    Serial.println(gps.location.lng(), 6); // Imprimir longitud con 6 decimales
  }

  // Lectura de la brújula
  compass.read();
  int x = compass.getX();
  int y = compass.getY();

  heading = atan2(y, x) * 180.0 / PI;
  if (heading < 0) heading += 360;

  heading -= declinacion;
  if (heading < 0) heading += 360;

  Serial.print("Heading: ");
  Serial.print(heading);
  Serial.println("°");

  // Lectura del sensor ultrasonico
  distance = checkdistance();
  server.handleClient();

  // Actualización de estados del motor
  if (!State2 && !State1 && !State0) { // STOP
    MotorStop(1);
    MotorStop(2);
  }
  if (!State2 && !State1 && State0) { // FORWARD
    controlMotor(1, true, 100);
    controlMotor(2, true, 100);
  }
  if (!State2 && State1 && !State0) { // BACKWARD
    controlMotor(1, false, 100);
    controlMotor(2, false, 100);
  }
  if (!State2 && State1 && State0) { // LEFT
    MotorStop(1);
    controlMotor(2, true, 100);
  }
  if (State2 && !State1 && !State0) { // RIGHT
    controlMotor(1, true, 100);
    MotorStop(2);
  }
}

// Funciones del sensor ultrasonico
float checkdistance() {
  digitalWrite(USTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(USTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(USTrigPin, LOW);
  duration = pulseIn(USEchoPin, HIGH);
  distance = duration * soundSpeed / 2;
  return distance;
}

// Funciones del motor
void setMotor(int in1, int in2, int en, bool direction, int velocidad) {
  digitalWrite(in1, direction ? HIGH : LOW);
  digitalWrite(in2, direction ? LOW : HIGH);
  analogWrite(en, velocidad);
}

void stopMotor(int in1, int in2, int en) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(en, 0);
}

void controlMotor(int motor, bool direction, int velocidad) {
  if (motor == 1) {
    setMotor(IN1Pin, IN2Pin, EN1Pin, direction, velocidad);
  } else if (motor == 2) {
    setMotor(IN3Pin, IN4Pin, EN2Pin, direction, velocidad);
  }
}

void MotorStop(int motor) {
  if (motor == 1) {
    stopMotor(IN1Pin, IN2Pin, EN1Pin);
  } else if (motor == 2) {
    stopMotor(IN3Pin, IN4Pin, EN2Pin);
  }
}

// Funciones de la brújula
float calcularBearing(float lat1, float lon1, float lat2, float lon2) {
  lat1 = radians(lat1);
  lon1 = radians(lon1);
  lat2 = radians(lat2);
  lon2 = radians(lon2);

  float dLon = lon2 - lon1;
  float y = sin(dLon) * cos(lat2);
  float x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
  float bearing = atan2(y, x);

  bearing = degrees(bearing);
  if (bearing < 0) {
    bearing += 360;
  }

  return bearing;
}

// Funciones del servidor web
void sendResponse(bool state2, bool state1, bool state0) {
  State2 = state2;
  State1 = state1;
  State0 = state0;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_OnConnect() {sendResponse(LOW, LOW, LOW);}
void handle_stop() { if ((millis() - lastDebounceTime) > debounceDelay) { sendResponse(LOW, LOW, LOW); lastDebounceTime = millis(); } }
void handle_adelante() { if ((millis() - lastDebounceTime) > debounceDelay) { sendResponse(LOW, LOW, HIGH); lastDebounceTime = millis(); } }
void handle_atras() { if ((millis() - lastDebounceTime) > debounceDelay) { sendResponse(LOW, HIGH, LOW); lastDebounceTime = millis(); } }
void handle_izquierda() { if ((millis() - lastDebounceTime) > debounceDelay) { sendResponse(LOW, HIGH, HIGH); lastDebounceTime = millis(); } }
void handle_derecha() { if ((millis() - lastDebounceTime) > debounceDelay) { sendResponse(HIGH, LOW, LOW); lastDebounceTime = millis(); } }
void handle_NotFound() {server.send(404, "text/plain", "Page Not Found");}