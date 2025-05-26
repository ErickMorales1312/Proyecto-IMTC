/* --- Librerías --- */  

#include <WiFi.h> 

#include <WiFiClientSecure.h> 

#include <PubSubClient.h> 

#include <Wire.h> 

#include <QMC5883LCompass.h> 

#include <TinyGPS++.h> 

#include <math.h> 

#include "time.h" 

 

/* --- Defines --- */  

// Pines para el sensor ultrasónico 

#define USTrigPin 26 

#define USEchoPin 27 

// Pines para el GPS 

#define RXPin 16 

#define TXPin 17 

// Pines para el L298N 

#define EN1Pin 4 

#define EN2Pin 32 

#define IN1Pin 18 

#define IN2Pin 19 

#define IN3Pin 33 

#define IN4Pin 25 

// Pin para la bomba 

#define BombaPin 23 

// Velocidad de transmisión de datos con el GPS y declinación en Monterrey 

#define GPS_Baud 9600 

#define declinacion 4.44 

// Tamaño del buffer para la recepción de mensajes 

#define INCOMING_DATA_BUFFER_SIZE 32 

 

/* --- Datos de conectividad --- */  

// Credenciales de red 

const char* ssid = ""; 

const char* password =  ""; 

// Credenciales Azure 

const char* iotHubName = ""; 

const char* deviceId = ""; 

const char* sasToken = ""; 

const char* mqttServer = iotHubName; 

const int mqttPort = 8883; 

String mqttUsername = ""; 

// Certificado raíz usado por Azure 

const char* azure_root_ca = \ 

"-----BEGIN CERTIFICATE-----\n" \ 

"MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n" \ 

"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \ 

"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \ 

"MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \ 

"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \ 

"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n" \ 

"9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n" \ 

"2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n" \ 

"1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n" \ 

"q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n" \ 

"tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n" \ 

"vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n" \ 

"BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n" \ 

"5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n" \ 

"1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n" \ 

"NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n" \ 

"Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n" \ 

"8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n" \ 

"pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n" \ 

"MrY=\n" \ 

"-----END CERTIFICATE-----\n"; 

// Variables para establecer hora local 

#define NTP_SERVERS "pool.ntp.org", "time.google.com", "time.windows.com" 

const long gmtOffset_sec = -21600; 

const int daylightOffset_sec = 0; 

 

/* --- Constantes --- */ 

// Velocidades (0-255) 

const int VELOCIDAD_AVANCE = 240; 

const int VELOCIDAD_GIRO = 180; 

// Tolerancias 

const double TOLERANCIA_GRADOS = 17.0; 

const double TOLERANCIA_METROS = 3; 

const double TOLERANCIA_OBSTACULO = 20; // cm 

// Constante de control proporcional de giro 

const double KP_CONTROL = 0.033; 

// Tiempos (segundos) 

const double TIEMPO_EVASION = 1; 

const double TIEMPO_MINIMO_GIRO = 0.2; 

// Constantes para cálculos de distancias 

const double RADIO_TIERRA = 6371000.0; // en metros 

const double GRADOS_LATITUD_METROS = 111320.0; // Aproximado 

// Velocidad del sonido para calcular distancias con el sensor ultrasónico 

const double soundSpeed = 0.034; 

 

/* --- Variables --- */ 

// Variables de clases 

TinyGPSPlus gps; 

QMC5883LCompass compass; 

HardwareSerial gpsSerial(2); 

WiFiClientSecure secureClient; 

PubSubClient mqttClient(secureClient); 

// Variables relacionadas a la orientación 

double bearing = 0.0; 

double heading = 0.0; 

int minX = 32767, maxX = -32768; 

int minY = 32767, maxY = -32768; 

// Variables relacionadas a la localización 

int satelites = 0; 

int hdop = 0; 

double latitud = 0.0; 

double longitud = 0.0; 

double latitudObj = 0.0; 

double longitudObj = 0.0; 

double latitudIni = 0.0; 

double longitudIni = 0.0; 

// Variables auxiliares para la reconstrucción de mensajes 

int latitudObjEntero = 0; 

int latitudObjDecimal = 0; 

int longitudObjEntero = 0; 

int longitudObjDecimal = 0; 

// Variables para controlar los tiempos de giro (en base a la Kp) 

double tiempoDeGiro = 0.0; 

unsigned long tiempoGiroStart = 0; 

// Variable para almacenar distancia al objetivo (módulo o HOME) 

double distanciaAlObj = 0.0; 

// Variables para el sensor ultrasónico 

double distance = 0;  

long duration; 

// Flags 

bool mensaje = false; 

bool conectado = false; 

bool dataIsValid = false; 

bool enGiro = false; 

 

// SETUP 

void setup() 

{ 

  // Inicializar comunicación Serial y conectar con el IoT Hub 

  Serial.begin(115200); 

  Connect(); 

 

  //Configuración del GPS 

  gpsSerial.begin(GPS_Baud, SERIAL_8N1, RXPin, TXPin); 

 

  // Configuración de la brújula 

  Wire.begin(); 

  compass.init(); 

 

  // Configuración del motor L298N 

  pinMode(EN1Pin, OUTPUT); 

  pinMode(EN2Pin, OUTPUT); 

  pinMode(IN1Pin, OUTPUT); 

  pinMode(IN2Pin, OUTPUT); 

  pinMode(IN3Pin, OUTPUT); 

  pinMode(IN4Pin, OUTPUT); 

 

  // Configuración del sensor Ultrasonico 

  pinMode(USTrigPin, OUTPUT); 

  pinMode(USEchoPin, INPUT); 

 

  // Configuración de la Bomba 

  pinMode(BombaPin, OUTPUT); 

 

  // Confirmar que terminó el setup por Serial 

  Serial.println("Listo el Setup"); 

} 

 

// LOOP 

void loop() 

{ 

  // Modo: en espera de mensaje 

  if (!mensaje && conectado) 

  { 

    mqttClient.loop(); 

  } 

 

  // Modo: calibrar sensores (al recibir un mensaje) 

  else if (mensaje && conectado) 

  { 

    Serial.println("Desconetar WiFi, calibrar brujula y comprobar sensores"); 

     

    // Desconectar el cliente MQTT y apagar el WiFi 

    Disconnect(); 

    delay(2000); 

 

    // Calibrar la brujula 

    MoveLeft(VELOCIDAD_GIRO); 

    delay(20000); 

    MoveRight(VELOCIDAD_GIRO); 

    delay(20000); 

    Stop(); 

 

    // Esperar a que los sensores den lecturas válidas 

    while (!dataIsValid) 

    { 

      // Lectura del GPS 

      while (gpsSerial.available() > 0) 

      { 

        gps.encode(gpsSerial.read()); 

      } 

 

      // Extraer valores del gps 

      satelites = gps.satellites.value(); 

      hdop = gps.hdop.value(); 

      latitud = redondearDecimales(gps.location.lat(), 7); 

      longitud = redondearDecimales(gps.location.lng(), 7); 

 

      // Lectura de la brújula 

      compass.read(); 

      int x = compass.getX(); 

      int y = compass.getY(); 

 

      // Guardar máximos y mínimos 

      if (x < minX) minX = x; 

      if (x > maxX) maxX = x; 

      if (y < minY) minY = y; 

      if (y > maxY) maxY = y; 

 

      // Calcular offsets 

      int offsetX = (maxX + minX) / 2; 

      int offsetY = (maxY + minY) / 2; 

 

      // Corregir componentes 

      int xCorr = x - offsetX; 

      int yCorr = y - offsetY; 

 

      // Calcular heading 

      heading = atan2(yCorr, xCorr) * 180.0 / PI; 

      if (heading < 0) heading += 360.0; 

      heading -= declinacion; 

      if (heading < 0) heading += 360.0; 

       

      // Redondear el heading a 2 decimales 

      heading = redondearDecimales(heading, 2); 

 

      // Imprimir lecturas de GPS y brujula 

      Serial.println("Satelites = " + String(satelites) + "\nHDOP = " + String(hdop) + "\nCoordenadas = " + String(latitud) +  ", " + String(longitud) + "\nHeading = " + String(heading)); 

 

      // Verificar que las lecturas sean válidas 

      if ((heading >= 355.7 || heading <= 355.4) && (satelites >= 7 && hdop <= 180)) 

      { 

        // Guardar latitud y longitud iniciales para el regreso a HOME 

        latitudIni = latitud; 

        longitudIni = longitud; 

        // Imprimir que las lecturas son válidas y actualizar flag 

        Serial.println("Data is Valid"); 

        dataIsValid = true; 

      } 

    } 

  } 

   

  // Modo: dirigirse al modulo 

  else if (mensaje && !conectado) 

  { 

    // Lectura del GPS 

    while (gpsSerial.available() > 0) 

    { 

      gps.encode(gpsSerial.read()); 

    } 

 

    // Obtener latitud y longitud 

    latitud = redondearDecimales(gps.location.lat(), 7); 

    longitud = redondearDecimales(gps.location.lng(), 7); 

 

    // Lectura de la brújula 

    compass.read(); 

    int x = compass.getX(); 

    int y = compass.getY(); 

 

    // Guardar máximos y mínimos 

    if (x < minX) minX = x; 

    if (x > maxX) maxX = x; 

    if (y < minY) minY = y; 

    if (y > maxY) maxY = y; 

 

    // Calcular offsets 

    int offsetX = (maxX + minX) / 2; 

    int offsetY = (maxY + minY) / 2; 

 

    // Corregir valores 

    int xCorr = x - offsetX; 

    int yCorr = y - offsetY; 

 

    // Calcular heading 

    heading = atan2(yCorr, xCorr) * 180.0 / PI; 

    if (heading < 0) heading += 360.0; 

    heading -= declinacion; 

    if (heading < 0) heading += 360.0; 

    // Redondear el heading a 2 decimales e imprimirlo 

    heading = redondearDecimales(heading, 2); 

    Serial.println("Heading: " + String(heading)); 

     

    // Calcular distancia al frente y evadir si existe algún obstáculo 

    distance = CheckDistance(); 

    if (distance > 0 && distance <= TOLERANCIA_OBSTACULO) 

    { 

      Serial.println("¡Obstáculo detectado! Ejecutando maniobra de evasión."); 

      evadirObstaculo(); 

      return; 

    } 

 

    // Calcular el bearing e imprimirlo 

    bearing = redondearDecimales(CalcularBearing(latitud, longitud, latitudObj, longitudObj), 2); 

    Serial.println("Bearing: " + String(bearing)); 

           

    // Avanzar o corregir direccion en base al bearing y heading 

    ControlDeMovimiento(bearing, heading); 

 

    // Calcular la distancia al modulo y determinar si se ha llegado 

    distanciaAlObj = CalcularDistancia(latitud, longitud, latitudObj, longitudObj); 

    Serial.print("Distancia al punto: "); 

    Serial.println(distanciaAlObj); 

    if (distanciaAlObj <= TOLERANCIA_METROS) 

    { 

      Serial.println("Llego al modulo"); 

 

      // Detener vehiculo y hacer rutina de la bomba 

      Stop(); 

      Serial.println("Encender bomba"); 

      digitalWrite(BombaPin, HIGH); 

      delay(10000); 

      digitalWrite(BombaPin, LOW); 

      Serial.println("Apagar bomba"); 

 

      // Actualizar flag 

      mensaje = false; 

    } 

  } 

 

  // Modo: volver a HOME 

  else if (!mensaje && !conectado) 

  { 

    // Lectura del GPS 

    while (gpsSerial.available() > 0) 

    { 

      gps.encode(gpsSerial.read()); 

    } 

 

    // Obtener latitud y longitud 

    latitud = redondearDecimales(gps.location.lat(), 7); 

    longitud = redondearDecimales(gps.location.lng(), 7); 

 

    // Lectura de la brújula 

    compass.read(); 

    int x = compass.getX(); 

    int y = compass.getY(); 

 

    // Guardar máximos y mínimos 

    if (x < minX) minX = x; 

    if (x > maxX) maxX = x; 

    if (y < minY) minY = y; 

    if (y > maxY) maxY = y; 

 

    // Calcular offset 

    int offsetX = (maxX + minX) / 2; 

    int offsetY = (maxY + minY) / 2; 

 

    // Corregir valores 

    int xCorr = x - offsetX; 

    int yCorr = y - offsetY; 

 

    // Calcular heading 

    heading = atan2(yCorr, xCorr) * 180.0 / PI; 

    if (heading < 0) heading += 360.0; 

    heading -= declinacion; 

    if (heading < 0) heading += 360.0; 

    // Redondear el heading a 2 decimales e imprimirlo 

    heading = redondearDecimales(heading, 2); 

    Serial.println("Heading: " + String(heading)); 

     

    // Calcular distancia al frente y evadir si existe algún obstáculo 

    distance = CheckDistance(); 

    if (distance > 0 && distance <= TOLERANCIA_OBSTACULO) 

    { 

      Serial.println("¡Obstáculo detectado! Ejecutando maniobra de evasión."); 

      evadirObstaculo(); 

      return; 

    } 

 

    // Calcular el bearing e imprimirlo 

    bearing = redondearDecimales(CalcularBearing(latitud, longitud, latitudIni, longitudIni), 2); 

    Serial.println("Bearing: " + String(bearing)); 

           

    // Avanzar o corregir direccion en base al bearing y heading 

    ControlDeMovimiento(bearing, heading); 

 

    // Calcular la distancia a HOME y determinar si se ha llegado 

    distanciaAlObj = CalcularDistancia(latitud, longitud, latitudIni, longitudIni); 

    Serial.print("Distancia a HOME: "); 

    Serial.println(distanciaAlObj); 

    if (distanciaAlObj <= TOLERANCIA_METROS) 

    { 

      Serial.println("ESTOY EN HOME"); 

 

      // Detener vehiculo 

      Stop(); 

 

      // Reconectar al IoT Hub 

      Connect(); 

    } 

  } 

} 

 

// Callback para mensajes C2D 

void callback(char* topic, byte* payload, unsigned int length) 

{ 

  // Imprimir topic y contenido del mensaje 

  Serial.print("Mensaje recibido en topic: "); 

  Serial.println(topic); 

  Serial.print("Contenido: "); 

  for (int i = 0; i < length; i++) { 

    Serial.print((char)payload[i]); 

  } 

  Serial.println(); 

 

  // Definir un buffer para el mensaje recibido 

  static char incoming_data[INCOMING_DATA_BUFFER_SIZE]; 

 

  // Copiar el payload (byte*) a incoming_data (char array) y agregar el terminador nulo 

  memcpy(incoming_data, payload, length+1); 

  incoming_data[length+1] = '\0'; 

 

  // Dividir el mensaje en 4 partes 

  char *token = strtok(incoming_data, ","); 

  if (token != NULL) latitudObjEntero = atoi(token); 

 

  token = strtok(NULL, ","); 

  if (token != NULL) latitudObjDecimal = atoi(token); 

 

  token = strtok(NULL, ",");   

  if (token != NULL) longitudObjEntero = atoi(token); 

 

  token = strtok(NULL, ","); 

  if (token != NULL) longitudObjDecimal = atoi(token); 

 

  // Reconstruir los datos 

  latitudObj = latitudObjEntero + (latitudObjDecimal / 10000000.0); 

  longitudObj = longitudObjEntero + (longitudObjDecimal / 10000000.0); 

 

  // Corregir signo si parte entera es negativa 

  if (latitudObjEntero < 0) 

  { 

    latitudObj -= (latitudObjDecimal / 10000000.0) * 2; 

  } 

  if (longitudObjEntero < 0) 

  { 

    longitudObj -= (longitudObjDecimal / 10000000.0) * 2; 

  } 

 

  // Imprimir latitud y longitud del mensaje 

  Serial.print("Latitud: ") 

  Serial.println(latitudObj,7); 

  Serial.print("Longitud: ") 

  Serial.println(longitudObj,7); 

 

  // Actualizar flag 

  mensaje = true; 

} 

 

// Funcion para calcular el Bearing 

double CalcularBearing(float lat1, float lon1, float lat2, float lon2) 

{ 

  // Convierte latitudes y longitudes a radianes 

  lat1 = radians(lat1); 

  lon1 = radians(lon1); 

  lat2 = radians(lat2); 

  lon2 = radians(lon2); 

 

  // Calcula el bearing 

  double dLon = lon2 - lon1; 

  double y = sin(dLon) * cos(lat2); 

  double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon); 

  double bearing = atan2(y, x); 

 

  // Convierte el bearing a grados 

  bearing = degrees(bearing); 

  // Corrgie el bearing 

  if (bearing < 0) 

  { 

    bearing += 360; 

  } 

 

  return bearing; 

} 

 

// Funcion para controlar el movimiento del vehiculo en base al bearing y heading 

void ControlDeMovimiento(double bearing, double heading) 

{ 

  // Calcular el error entre el bearing y el heading con 2 decimales 

  double error = redondearDecimales((bearing - heading), 2); 

 

  // Ajustar el error considerando la naturaleza cíclica de los ángulos 

  if (error > 180) 

  { 

    error -= 360; 

  } 

  else if (error < -180) 

  { 

    error += 360; 

  } 

 

  // Imprimir el error calculado 

  Serial.print("Error: "); 

  Serial.println(error, 2); 

  delay(200); 

 

  // Verificar si el heading esta dentro de la tolerancia 

  // Dentro de tolerancia --> Avanzar recto 

  if (abs(error) <= TOLERANCIA_GRADOS) 

  { 

    if (enGiro) 

    { 

      enGiro = false; 

      Serial.println("Giro completado, avanzando recto"); 

    } 

    MoveForward(VELOCIDAD_AVANCE); 

  } 

  // Fuera de tolerancia --> Corregir heading 

  else 

  { 

    if (!enGiro) 

    { 

      tiempoDeGiro = KP_CONTROL * abs(error); 

      if (tiempoDeGiro < TIEMPO_MINIMO_GIRO) tiempoDeGiro = TIEMPO_MINIMO_GIRO; 

      tiempoGiroStart = millis(); 

      enGiro = true; 

 

      if (error > 0) 

      { 

        Serial.println("Girando a la derecha"); 

        MoveRight(VELOCIDAD_GIRO); 

      }  

      else 

      { 

        Serial.println("Girando a la izquierda"); 

        MoveLeft(VELOCIDAD_GIRO); 

      } 

    } 

    else 

    { 

      if (millis() - tiempoGiroStart >= tiempoDeGiro * 1000) 

      { 

        enGiro = false; 

      } 

    } 

  } 

} 

 

// Funcion para calcular la distancia entre el vehiculo y la ubicacion objetivo 

double CalcularDistancia(double lat1, double lon1, double lat2, double lon2) 

{ 

  // Calcular las diferencias de latitud y longitud 

  double deltaLat = lat2 - lat1; 

  double deltaLon = lon2 - lon1; 

 

  // Promediar de la latitud para corregir el tamaño de 1° de longitud 

  double latPromedio = (lat1 + lat2) / 2.0; 

  double metrosPorGradoLon = GRADOS_LATITUD_METROS * cos(radians(latPromedio)); 

 

  // Convertir diferencias de grados a metros 

  double deltaLatMetros = deltaLat * GRADOS_LATITUD_METROS; 

  double deltaLonMetros = deltaLon * metrosPorGradoLon; 

 

  // Distancia euclidiana (pitagoras) 

  double distancia = sqrt(deltaLatMetros * deltaLatMetros + deltaLonMetros * deltaLonMetros); 

 

  return distancia; // en metros 

} 

 

// Función para medir la distancia usando el sensor ultrasónico 

double CheckDistance() 

{ 

  // Limpia el USTrigPin  

  digitalWrite(USTrigPin, LOW); 

  delayMicroseconds(2); 

  // Pone en HIGH USTrigPin por 10 uS 

  digitalWrite(USTrigPin, HIGH); 

  delayMicroseconds(10); 

  digitalWrite(USTrigPin, LOW); 

  float measure = pulseIn(USEchoPin, HIGH) * soundSpeed / 2; 

  delay(10); 

  return measure; // en cm 

} 

 

// Funcion con rutina para evadir un obstaculo 

void evadirObstaculo() 

{ 

  MoveBackward(VELOCIDAD_AVANCE); 

  delay(TIEMPO_EVASION * 1000); 

  MoveLeft(VELOCIDAD_GIRO); 

  delay(TIEMPO_EVASION * 1000); 

  MoveForward(VELOCIDAD_AVANCE); 

  delay(TIEMPO_EVASION * 1000); 

} 

 

// Funcion para conectar WiFi y al IoT Hub 

void Connect() 

{ 

  // Conectar al WiFi 

  WiFi.begin(ssid, password); 

  Serial.print("Conectando a WiFi"); 

  while (WiFi.status() != WL_CONNECTED)  

  { 

    delay(500); 

    Serial.print("."); 

  } 

  Serial.println("Conectado a WiFi"); 

 

  // Sincronizar hora con NTP 

  configTime(gmtOffset_sec, daylightOffset_sec, NTP_SERVERS); 

  Serial.println("Sincronizando hora con NTP..."); 

  printLocalTime(); 

  // Esperar hasta que la hora esté sincronizada 

  struct tm timeinfo; 

  while (!getLocalTime(&timeinfo)) { 

    delay(500); 

    Serial.print("."); 

  } 

  Serial.println("\nHora sincronizada."); 

  printLocalTime(); 

 

  // Imprimir IP del IoT Hub 

  IPAddress hubIp; 

  if (WiFi.hostByName(mqttServer, hubIp)) { 

    Serial.print("IP del IoT Hub: "); 

    Serial.println(hubIp); 

  }  

  else 

  { 

    Serial.println("No se pudo resolver el nombre del IoT Hub."); 

  } 

 

  // TLS setup 

  secureClient.setCACert(azure_root_ca); 

 

  // MQTT setup 

  mqttClient.setServer(mqttServer, mqttPort); 

  secureClient.setHandshakeTimeout(30);  // Extiende timeout para TLS 

  secureClient.setTimeout(30);           // Establece timeout general 

  mqttClient.setCallback(callback); 

 

  Serial.println("Conectando a Azure IoT Hub..."); 

  while (!mqttClient.connected()) 

  { 

    if (mqttClient.connect(deviceId, mqttUsername.c_str(), sasToken)) 

    { 

      Serial.println("Conectado al IoT Hub"); 

 

      // Suscribirse a mensajes C2D 

      String topic = "devices/" + String(deviceId) + "/messages/devicebound/#"; 

      mqttClient.subscribe(topic.c_str()); 

      Serial.println("Suscrito a mensajes C2D"); 

    } 

    else  

    { 

      Serial.print("Error al conectar. Código: "); 

      Serial.println(mqttClient.state()); 

      delay(2000); 

    } 

  } 

 

  // Actualizar flag 

  conectado = true; 

} 

 

// Funcion para desconectar del IoT Hub y apagar WiFi 

void Disconnect() 

{ 

  mqttClient.disconnect(); 

  WiFi.disconnect(true); 

  WiFi.mode(WIFI_OFF); 

 

  // Actualizar flag 

  conectado = false; 

} 

 

// Función para redondear doubles a 2 decimales 

double redondearDecimales(double numero, double decimales) 

{ 

    double factor = pow(10, decimales); 

    return roundf(numero * factor) / factor; 

} 

 

// Funciones para controlar el movimiento del vehiculo 

void Stop() 

{ 

 RightMotorOff(); 

 LeftMotorOff();  

} 

void MoveForward(int vel) 

{ 

  RightMotorMoveForward(vel); 

  LeftMotorMoveForward(vel-10); 

} 

void MoveBackward(int vel) 

{ 

  RightMotorMoveBackward(vel); 

  LeftMotorMoveBackward(vel-10); 

} 

void MoveLeft(int vel) 

{ 

  LeftMotorMoveBackward(vel); 

  RightMotorMoveForward(vel); 

} 

void MoveRight(int vel) 

{ 

  RightMotorMoveBackward(vel); 

  LeftMotorMoveForward(vel); 

} 

// Funciones para controlar los motores derechos 

void RightMotorMoveForward(int vel) 

{ 

  digitalWrite(IN1Pin, HIGH); 

  digitalWrite(IN2Pin, LOW); 

  analogWrite(EN1Pin, vel); 

} 

void RightMotorMoveBackward(int vel) 

{ 

  digitalWrite(IN1Pin, LOW); 

  digitalWrite(IN2Pin, HIGH); 

  analogWrite(EN1Pin, vel); 

} 

void RightMotorStop() 

{ 

  digitalWrite(IN1Pin, HIGH); 

  digitalWrite(IN2Pin, LOW); 

  analogWrite(EN1Pin, 40); 

} 

void RightMotorOff() 

{ 

  digitalWrite(IN1Pin, LOW); 

  digitalWrite(IN2Pin, LOW); 

  analogWrite(EN1Pin, 0); 

} 

 

void StopMotors() 

{ 

  digitalWrite(IN1Pin, HIGH); 

  digitalWrite(IN2Pin, LOW); 

  analogWrite(EN1Pin, 40); 

  digitalWrite(IN3Pin, HIGH); 

  digitalWrite(IN4Pin, LOW); 

  analogWrite(EN2Pin, 40); 

} 

 

// Funciones para controlar los motores izquierdos 

void LeftMotorMoveForward(int vel) 

{ 

  digitalWrite(IN3Pin, HIGH); 

  digitalWrite(IN4Pin, LOW); 

  analogWrite(EN2Pin, vel); 

} 

void LeftMotorMoveBackward(int vel) 

{ 

  digitalWrite(IN3Pin, LOW); 

  digitalWrite(IN4Pin, HIGH); 

  analogWrite(EN2Pin, vel); 

} 

void LeftMotorStop() 

{ 

  digitalWrite(IN3Pin, HIGH); 

  digitalWrite(IN4Pin, LOW); 

  analogWrite(EN2Pin, 40); 

} 

void LeftMotorOff() 

{ 

  digitalWrite(IN3Pin, LOW); 

  digitalWrite(IN4Pin, LOW); 

  analogWrite(EN2Pin, 0); 

} 

 

// Funcion para imprimir la hora local 

void printLocalTime() 

{ 

  struct tm timeinfo; 

  if (!getLocalTime(&timeinfo)) 

  { 

    Serial.println("Fallo al obtener la hora local"); 

    return; 

  } 

  Serial.printf("Hora local: %02d:%02d:%02d %02d/%02d/%04d\n", 

                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, 

                timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900); 

}