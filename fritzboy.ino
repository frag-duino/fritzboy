/*
 * Fritz!Boy Request-Client
 * Works only with Arduino Mega and Ethernet shield
 * 2015 for "Pimp your Fritz" Marcel Imig
 * www.frag-duino.de
 *
 * Connections:
 * Link Cable     Arduino      Desc
 * 6              GND          GND
 * 5              38           SCK
 * 2              x            SIN (not used)
 * 3              36           SOUT
*/

// Includes
#include <SPI.h>
#include <Ethernet.h>

// Config
#define BUFFER_SIZE 50 // Buffer-size for HTTP-responses
#define UNDEFINED 1337
#define DELAYBETWEENBITS 60
int PIN_GND = 40;   // If not used "GND" (blue)
int PIN_CLOCK = 38; // Clock Pin for Gameboy (green)
int PIN_SO = 36;    // SO Pin for Gameboy (red)
int PIN_LED = 13;   // Status LED
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xEF, 0xAE }; // MAC-Address of the adapter
const int POLLING_INTERVAL = 3000;  // Interval for getting current bandwidth (ms)
const int PUSHING_INTERVAL = 1000; // Interval for pushing the current values to gameboy
const int SENDING_DELAY = 200; // Delay between the sending of HTTP-Lines

// Variables
EthernetClient client;
String request = "";           // HTTP-Request String
char response[BUFFER_SIZE];    // Buffer for HTTP-Response
String router_ip = "";         // IP of the router
unsigned long rate_receive_abs = UNDEFINED;    // Current absolute receiving rate
unsigned long rate_send_abs = UNDEFINED;       // Current absolute sending rate
unsigned long rate_receive_percent = UNDEFINED;// Current percentage receiving rate
unsigned long rate_send_percent = UNDEFINED;   // Current percentage sending rate
unsigned long maximum_upstream_long = UNDEFINED;    // Maximum upstream bandwidth
unsigned long maximum_downstream_long = UNDEFINED;  // Maximum downstream bandwidth (51000000)
unsigned long maximum_upstream_short = UNDEFINED;    // Maximum upstream bandwidth
unsigned long maximum_downstream_short = UNDEFINED;  // Maximum downstream bandwidth (51)
int lastIntegerPosition = -1;  // Temp Variables for searching the value
int firstIntegerPosition = -1; // Temp Variables for searching the value
char * pch;                    // Temp Variables for searching the value
unsigned long last_request_send = 0; // Timestamp of last request sent
unsigned long last_push_values = 0; // Timestamp of last push of the current bandwidth
unsigned long currentMillis = 0;     // Current timestamp
byte outbuffer[10]; // Buffer for sending
boolean response_found_downstream = false; // True if downstream found
boolean response_found_upstream = false;   // True if upstream found
unsigned long temp = 0; // for incoming values
byte temp_a = 0;
byte temp_b = 0;

void setup() {
  // Initialize Pins
  pinMode(PIN_GND, OUTPUT);
  digitalWrite(PIN_GND, LOW);
  pinMode(PIN_SO, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);


  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Fritz!Boy 1.0 starting, waiting for DHCP");

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    errorstate();
  }
  delay(500);

  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("GW: ");
  router_ip = convertAddress(Ethernet.gatewayIP());
  Serial.println(router_ip);
  sendRequest_maximumbandwidth(); // Send request for maximum bandwidth
  resetBuffer(); // Clear the buffer
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  // read incoming bytes from HTTP-Response
  while (client.available()) {
    char c = client.read();
    // Serial.print(c);

    // Arrayshift to the left and insert input
    for (int i = 0; i < BUFFER_SIZE - 1; i++)
      response[i] = response[i + 1];
    response[BUFFER_SIZE - 1] = c;

    if (handle_response())
      break;
  }

  // if the server's disconnected, stop the client:
  if (client.connected()) {
    client.flush();
    Serial.println("CLOSE");
    client.stop();
    resetBuffer();
    if (response_found_downstream == false || response_found_upstream == false){
      Serial.println("Connection Error. Retrying!"); // Request was too fast
      last_request_send = 0;
    }
  }

  response_found_upstream = false;
  response_found_downstream = false;

  // Get current time
  currentMillis = millis();

  // Send bandwidthusage request
  if (last_request_send + POLLING_INTERVAL < currentMillis) {
    sendRequest_bandwidthusage();
    last_request_send = currentMillis;
  }

  // Send current values to gameboy
  if (last_push_values + PUSHING_INTERVAL < currentMillis) {
    pushValues(); // Send Values to gameboy
    last_push_values = currentMillis;
  }
}
