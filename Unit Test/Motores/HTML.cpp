#include "HTML.h"

String SendHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0) {
  String html = "<!DOCTYPE html><html>\n";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  html += "<meta http-equiv=\"refresh\" content=\"2\">\n";
  html += "<title>Carrito</title>\n";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  html += "body { margin-top: 50px; } h1 { color: #444444; margin: 50px auto 30px; } h3 { color: #444444; margin-bottom: 10px; }\n";
  html += ".button { display: block; width: 80px; background-color: #3498db; border: none; color: white; padding: 13px 30px; text-decoration: none; font-size: 25px; margin: 0px auto 35px; cursor: pointer; border-radius: 4px; }\n";
  html += ".button-on { background-color: #3498db; }\n";
  html += ".button-on:active { background-color: #2980b9; }\n";
  html += ".button-off { background-color: #34495e; }\n";
  html += ".button-off:active { background-color: #2c3e50; }\n";
  html += "p { font-size: 14px; color: #888; margin-bottom: 10px; }\n";
  html += "</style>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<h1>Carrito</h1>\n";
  html += "<h3>Sensores</h3>\n";
  html += "<br><br>\n";
  html += "<h3>Control</h3>\n";

  // Generación de botones según el estado
  html += generateButtonHTML(Status2, Status1, Status0);

  html += "</body>\n";
  html += "</html>\n";
  return html;
}

String generateButtonHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0) {
  String buttonHTML = "";
  buttonHTML += "<a class=\"button " + String((!Status2 && !Status1 && !Status0) ? "button-on" : "button-off") + "\" href=\"/stop\">STOP</a>\n";
  buttonHTML += "<a class=\"button " + String((!Status2 && !Status1 && Status0) ? "button-on" : "button-off") + "\" href=\"/adelante\">Adelante</a>\n";
  buttonHTML += "<a class=\"button " + String((!Status2 && Status1 && !Status0) ? "button-on" : "button-off") + "\" href=\"/atras\">Atras</a>\n";
  buttonHTML += "<a class=\"button " + String((!Status2 && Status1 && Status0) ? "button-on" : "button-off") + "\" href=\"/izquierda\">Izquierda</a>\n";
  buttonHTML += "<a class=\"button " + String((Status2 && !Status1 && !Status0) ? "button-on" : "button-off") + "\" href=\"/derecha\">Derecha</a>\n";
  return buttonHTML;
}
