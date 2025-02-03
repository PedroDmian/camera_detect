#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <SPI.h>

// Definir la red Wi-Fi
const char *ssid = "tu_red_wifi";            // Cambia por tu SSID
const char *password = "tu_contraseña_wifi"; // Cambia por tu contraseña Wi-Fi

// URL del servidor
const char *serverUrl = "http://localhost:3000/update"; // Cambia si es otro servidor

WiFiClient client;
HTTPClient http;

void setup()
{
  // Inicializa el puerto serie
  Serial.begin(115200);

  // Conecta a la red Wi-Fi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Esperando conexión...");
  }
  Serial.println("Conectado al WiFi");

  // Inicializa la tarjeta SD
  if (!SD.begin())
  {
    Serial.println("Error al iniciar la tarjeta SD");
    return;
  }

  // Abre el archivo en la tarjeta SD (debe estar en la raíz)
  File file = SD.open("/imagen.jpg");
  if (!file)
  {
    Serial.println("No se puede abrir el archivo");
    return;
  }

  // Muestra el tamaño del archivo
  Serial.print("Tamaño del archivo: ");
  Serial.println(file.size());

  // Prepara la solicitud HTTP POST
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");

  String body = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\n";
  body += "Content-Disposition: form-data; name=\"file\"; filename=\"imagen.jpg\"\r\n";
  body += "Content-Type: image/jpeg\r\n\r\n";

  // Envía los datos del archivo como multipart/form-data
  http.POST(body.c_str());

  // Enviar los datos del archivo
  while (file.available())
  {
    http.write(file.read());
  }

  // Finaliza la solicitud HTTP
  String boundary_end = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--\r\n";
  http.write(boundary_end.c_str());

  // Lee la respuesta del servidor
  int httpCode = http.GET(); // Usa GET para obtener la respuesta
  String payload = http.getString();

  // Muestra el código de la respuesta
  Serial.println("Código de respuesta HTTP: " + String(httpCode));
  Serial.println("Respuesta del servidor: " + payload);

  // Cierra el archivo
  file.close();
}

void loop()
{
  // No se necesita hacer nada en el loop
}
