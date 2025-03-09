#include <WiFi.h>
#include <WebServer.h>
#include "HTML.h"

int LDRValue1 = 0;
int LDRValue2 = 0;
float distance = 0; 
long duration; // Definición de la variable distance
bool State2 = LOW;
bool State1 = LOW;
bool State0 = LOW;

// Declaración de los pines utilizados
#define LDRPin1 34
#define LDRPin2 35
#define USTrigPin 33
#define USEchoPin 32
#define EN1Pin 16
#define EN2Pin 18
#define IN1Pin 4
#define IN2Pin 2
#define IN3Pin 5
#define IN4Pin 17

#define soundSpeed 0.034

const char* ssid = "IZZI-77B8";
const char* password = "E3ZI7TLRY8FM";

WebServer server(80);

void handle_OnConnect() {
  State2 = LOW;
  State1 = LOW;
  State0 = LOW;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_stop() {
  State2 = LOW;
  State1 = LOW;
  State0 = LOW;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_adelante() {
  State2 = LOW;
  State1 = LOW;
  State0 = HIGH;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_atras() {
  State2 = LOW;
  State1 = HIGH;
  State0 = LOW;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_izquierda() {
  State2 = LOW;
  State1 = HIGH;
  State0 = HIGH;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_derecha() {
  State2 = HIGH;
  State1 = LOW;
  State0 = LOW;
  server.send(200, "text/html", SendHTML(State2, State1, State0));
}

void handle_NotFound() {
  server.send(404, "text/plain", "La pagina no existe");
}

void setup() {
  Serial.begin(115200);
  pinMode(LDRPin1, INPUT);
  pinMode(LDRPin2, INPUT);
  pinMode(USEchoPin, INPUT);
  pinMode(USTrigPin, OUTPUT);
  pinMode(EN1Pin, OUTPUT);
  pinMode(EN2Pin, OUTPUT);
  pinMode(IN1Pin, OUTPUT);
  pinMode(IN2Pin, OUTPUT);
  pinMode(IN3Pin, OUTPUT);
  pinMode(IN4Pin, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 ("Conectado a ");
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
  LDRValue1 = analogRead(LDRPin1);
  LDRValue2 = analogRead(LDRPin2);
  distance = checkdistance();
  server.handleClient();
 
 // Actualizacion del estado de los pines de salida
  if ((!State2) & (!State1) & (!State0)) // STOP
  {
    MotorStop_Motor1();
    MotorStop_Motor2();
  }
  if ((!State2) & (!State1) & (State0)) // DELANTE
  {
    MotorHorario_Motor1(100);
    MotorHorario_Motor2(100);
  }
  if ((!State2) & (State1) & (!State0)) // ATRAS
  {
    MotorAntiHorario_Motor1(100);
    MotorAntiHorario_Motor2(100);
  }
  if ((!State2) & (State1) & (State0)) // IZQUIERDA
  {
    MotorStop_Motor1();
    MotorHorario_Motor2(100);
  }
  if ((State2) & (!State1) & (!State0)) // DERECHA
  {
    MotorHorario_Motor1(100);
    MotorStop_Motor2();
  }
}


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

// Función para girar el Motor 1 en sentido horario con control de velocidad
void MotorHorario_Motor1(int velocidad) {
  digitalWrite(IN1Pin, HIGH);
  digitalWrite(IN2Pin, LOW);
  analogWrite(EN1Pin, velocidad);
}

// Función para girar el Motor 1 en sentido antihorario con control de velocidad
void MotorAntiHorario_Motor1(int velocidad) {
  digitalWrite(IN1Pin, LOW);
  digitalWrite(IN2Pin, HIGH);
  analogWrite(EN1Pin, velocidad);
}

// Función para girar el Motor 2 en sentido horario con control de velocidad
void MotorHorario_Motor2(int velocidad) {
  digitalWrite(IN3Pin, HIGH);
  digitalWrite(IN4Pin, LOW);
  analogWrite(EN2Pin, velocidad);
}

// Función para girar el Motor 2 en sentido antihorario con control de velocidad
void MotorAntiHorario_Motor2(int velocidad) {
  digitalWrite(IN3Pin, LOW);
  digitalWrite(IN4Pin, HIGH);
  analogWrite(EN2Pin, velocidad);
}

// Función para detener el Motor 1
void MotorStop_Motor1() {
  digitalWrite(IN1Pin, LOW);
  digitalWrite(IN2Pin, LOW);
  analogWrite(EN1Pin, 0);
}

// Función para detener el Motor 2
void MotorStop_Motor2() {
  digitalWrite(IN3Pin, LOW);
  digitalWrite(IN4Pin, LOW);
  analogWrite(EN2Pin, 0);
}
