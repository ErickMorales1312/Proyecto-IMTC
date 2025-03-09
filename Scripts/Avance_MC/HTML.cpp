#include "HTML.h"

String SendHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0) {
  String html = "<!DOCTYPE html> <html>\n";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  html += "<meta http-equiv=\"refresh\" content=\"2\">\n";
  html += "<title>Carrito</title>\n";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  html += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 10px;}\n";
  html += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  html += ".button-on {background-color: #3498db;}\n";
  html += ".button-on:active {background-color: #2980b9;}\n";
  html += ".button-off {background-color: #34495e;}\n";
  html += ".button-off:active {background-color: #2c3e50;}\n";
  html += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  html += "</style>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<h1>Carrito</h1>";
  html += "<h3>Sensores</h3>";
  html += "<p>LDR Izquierda: " + String(LDRValue1) + "</p>";
  html += "<p>LDR Derecha: " + String(LDRValue2) + "</p>";
  html += "<p>Ultrasonic Distance: " + String(distance) + "cm</p>";
  html += "<br><br>";
  html += "<h3>Control</h3>";
  if ((!State2) & (!State1) & (!State0)) {
    html += "<a class=\"button button-on\" href=\"/stop\">STOP</a>";
    html += "<a class=\"button button-off\" href=\"/adelante\">Adelante</a>";
    html += "<a class=\"button button-off\" href=\"/atras\">Atras</a>";
    html += "<a class=\"button button-off\" href=\"/izquierda\">Izquierda</a>";
    html += "<a class=\"button button-off\" href=\"/derecha\">Derecha</a>";
  }
  if ((!Status2) & (!Status1) & (Status0)) {
    html += "<a class=\"button button-off\" href=\"/stop\">STOP</a>";
    html += "<a class=\"button button-on\" href=\"/adelante\">Adelante</a>";
    html += "<a class=\"button button-off\" href=\"/atras\">Atras</a>";
    html += "<a class=\"button button-off\" href=\"/izquierda\">Izquierda</a>";
    html += "<a class=\"button button-off\" href=\"/derecha\">Derecha</a>";
  }
  if ((!Status2) & (Status1) & (!Status0)) {
    html += "<a class=\"button button-off\" href=\"/stop\">STOP</a>";
    html += "<a class=\"button button-off\" href=\"/adelante\">Adelante</a>";
    html += "<a class=\"button button-on\" href=\"/atras\">Atras</a>";
    html += "<a class=\"button button-off\" href=\"/izquierda\">Izquierda</a>";
    html += "<a class=\"button button-off\" href=\"/derecha\">Derecha</a>";
  }
  if ((!Status2) & (Status1) & (Status0)) {
    html += "<a class=\"button button-off\" href=\"/stop\">STOP</a>";
    html += "<a class=\"button button-off\" href=\"/adelante\">Adelante</a>";
    html += "<a class=\"button button-off\" href=\"/atras\">Atras</a>";
    html += "<a class=\"button button-on\" href=\"/izquierda\">Izquierda</a>";
    html += "<a class=\"button button-off\" href=\"/derecha\">Derecha</a>";
  }
  if ((Status2) & (!Status1) & (!Status0)) {
    html += "<a class=\"button button-off\" href=\"/stop\">STOP</a>";
    html += "<a class=\"button button-off\" href=\"/adelante\">Adelante</a>";
    html += "<a class=\"button button-off\" href=\"/atras\">Atras</a>";
    html += "<a class=\"button button-off\" href=\"/izquierda\">Izquierda</a>";
    html += "<a class=\"button button-on\" href=\"/derecha\">Derecha</a>";
  }
  html += "</body>\n";
  html += "</html>\n";
  return html;
}