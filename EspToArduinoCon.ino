#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>

// Predefined messages
const String SETUP_ERROR = "!!ERROR!! SETUP: Unable to start SoftAP mode";
const String SETUP_SERVER_START = "SETUP: HTTP server started --> IP addr: ";
const String SETUP_SERVER_PORT = " on port: ";
const String INFO_NEW_CLIENT = "New client connected";
const String INFO_DISCONNECT_CLIENT = "Client disconnected";

// Wi-Fi credentials
const char *SSID = "SSID_Sori";
const char *PASS = "12345678";
const int HTTP_PORT_NO = 80;

// Initialize HTTP server
WiFiServer HttpServer(HTTP_PORT_NO);

// Variable to hold data received from Arduino Mega
String receivedData = "";
String multiLineData = "";
bool newBoardReady = false; // Flag to indicate new data availability

void setup() {
  Serial.begin(115200);         // For debugging
  Serial.println("Starting ESP32 setup...");

  // Initialize Serial2 for communication with Arduino Mega
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX = GPIO16, TX = GPIO17
  Serial.println("Serial2 initialized for communication with Arduino Mega.");

  // Set up Wi-Fi SoftAP
  Serial.println("Attempting to start Wi-Fi Access Point...");
  if (!WiFi.softAP(SSID, PASS)) {
    Serial.println(SETUP_ERROR);
    while (1);  // Halt execution
  }

  IPAddress accessPointIP = WiFi.softAPIP();
  String webServerInfoMessage = SETUP_SERVER_START + accessPointIP.toString() + SETUP_SERVER_PORT + HTTP_PORT_NO;

  HttpServer.begin();
  Serial.println(webServerInfoMessage);
}

void loop() {
  // Check for data from Arduino Mega
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '!') { // End of message detected
      Serial.println("New game scene received: ");
      Serial.println(multiLineData);
      newBoardReady = true; // Mark the new board as ready to be displayed
      break;
    } else {
      multiLineData += c; // Accumulate data
    }
  }

  // Check for incoming client requests
  WiFiClient client = HttpServer.available();
  if (client) {
    Serial.println(INFO_NEW_CLIENT);
    String request = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // Log the incoming request
        Serial.print(c);

        if (c == '\n' && request.indexOf("GET / ") >= 0) {
          // Serve HTML page
          Serial.println("\nServing HTML page to client...");
          String htmlPage = R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
              <title>ESP32 Multi-Line Data Tracker</title>
              <script>
                function updateData() {
                  fetch('/get-data')
                    .then(response => response.text())
                    .then(data => {
                      document.getElementById('data').textContent = data;
                    });
                }
                setInterval(updateData, 1000); // Update every second
              </script>
            </head>
            <body>
              <h1>Game Scene Tracker</h1>
              <pre id="data">Loading...</pre>
            </body>
            </html>
          )rawliteral";

          client.println("HTTP/1.1 200 OK\r\nContent-type:text/html\r\n\r\n");
          client.print(htmlPage);
          break;
        }

        if (c == '\n' && request.indexOf("GET /get-data") >= 0) {
          // Serve the multi-line data
          if (newBoardReady) {
            Serial.println("\nServing new game scene to client: " + multiLineData);
            client.println("HTTP/1.1 200 OK\r\nContent-type:text/plain\r\n\r\n");
            client.println(multiLineData);
            multiLineData = ""; // Clear the data after serving
            newBoardReady = false; // Reset the flag
          } else {
            client.println("HTTP/1.1 200 OK\r\nContent-type:text/plain\r\n\r\n");
            client.println("Waiting for new game scene...");
          }
          break;
        }
      }
    }
    client.stop();
    Serial.println(INFO_DISCONNECT_CLIENT);
  }
}
