#include <WiFi.h>
#include <WebServer.h>
#include "HTML.h"
#include <TinyGPS++.h>

// Variables
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;
bool State2 = LOW;
bool State1 = LOW;
bool State0 = LOW;
float Latitud = 0;
float Longitud = 0;
float HDOP = 0;
int Satellites = 0;
int contador = 0;

// Definición de pines
#define EN1Pin 4
#define EN2Pin 32
#define IN1Pin 18
#define IN2Pin 19
#define IN3Pin 33
#define IN4Pin 25
const int RXPin = 16; // Pin de recepción (RX) del GPS
const int TXPin = 17; // Pin de transmisión (TX) del GPS

//Constantes
#define GPSBaud 9600

// Web server
const char* ssid = "IZZI-77B8";
const char* password = "E3ZI7TLRY8FM";

IPAddress ip(192,168,0,100); 
IPAddress gateway(192,168,0,1); 
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

// Funciones del motor
void setMotor(int in1, int in2, int en, bool direction, int velocidad);
void stopMotor(int in1, int in2, int en);
void controlMotor(int motor, bool direction, int velocidad);
void MotorStop(int motor);

void setup() {
  Serial.begin(115200);

  // Configuración del motor L298N
  pinMode(EN1Pin, OUTPUT);
  pinMode(EN2Pin, OUTPUT);
  pinMode(IN1Pin, OUTPUT);
  pinMode(IN2Pin, OUTPUT);
  pinMode(IN3Pin, OUTPUT);
  pinMode(IN4Pin, OUTPUT);

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

  //Configuración del GPS
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  Serial.println("GPS Inicializado");

  server.begin();
}

void loop() {

  server.handleClient();
 
  unsigned long start = millis();

  while (millis() - start < 1000)
  {
    while (gpsSerial.available() > 0)
    {
      gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated() || contador == 10)
    {
      contador = 0;
      Latitud = Serial.println(gps.location.lat(), 6);
      Longitud = Serial.println(gps.location.lng(), 6);
      HDOP = Serial.println(gps.hdop.value() / 100.0); 
      Satellites= Serial.println(gps.satellites.value());
    }
  }
  contador++;

  // Actualización de estados del motor
  if (!State2 && !State1 && !State0) { // STOP
    MotorStop(1);
    MotorStop(2);
  }
  if (!State2 && !State1 && State0) { // FORWARD
    controlMotor(1, true, 230);
    controlMotor(2, true, 230);
  }
  if (!State2 && State1 && !State0) { // BACKWARD
    controlMotor(1, false, 230);
    controlMotor(2, false, 230);
  }
  if (!State2 && State1 && State0) { // LEFT
    controlMotor(1, true, 230);
    //MotorStop(2);
    controlMotor(2, false, 40);
  }
  if (State2 && !State1 && !State0) { // RIGHT
    controlMotor(1, false, 40);
    //MotorStop(1);
    controlMotor(2, true, 230);
  }
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

// Funciones del servidor web
void sendResponse(bool state2, bool state1, bool state0) {
  State2 = state2;
  State1 = state1;
  State0 = state0;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_OnConnect() {sendResponse(LOW, LOW, LOW);}
void handle_stop() {sendResponse(LOW, LOW, LOW);}
void handle_adelante() {sendResponse(LOW, LOW, HIGH);}
void handle_atras() {sendResponse(LOW, HIGH, LOW); }
void handle_izquierda() {sendResponse(LOW, HIGH, HIGH);}
void handle_derecha() {sendResponse(HIGH, LOW, LOW);}
void handle_NotFound() {server.send(404, "text/plain", "Page Not Found");}