/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/communication-between-two-esp32
 */

// ESP32 #2: TCP SERVER + AN LED + PIEZO
#include <WiFi.h>
#define LED_PIN 19
#define BUZZ_PIN 12  // ESP32 pin GPIO18 connected to LED
#define SERVER_PORT 4080

const char* ssid = "Norinn";             // YOUR WIFI SSID
const char* password = "Kakesjokolade";  // YOUR WIFI PASSWORD
bool buzzerOn = false;
long starttid = 0;
long target_tid = 0;
int state = 0;
WiFiServer TCPserver(SERVER_PORT);

void setup() {
  Serial.begin(9600);

  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(BUZZ_PIN, LOW);  //buzzer off
  pinMode(LED_PIN, OUTPUT);

  Serial.println("ESP32 #2: TCP SERVER + AN LED");

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print your local IP address:
  Serial.print("ESP32 #2: TCP Server IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("ESP32 #2: -> Please update the serverAddress in ESP32 #1 code");

  // Start listening for a TCP client (from ESP32 #1)
  TCPserver.begin();
}

void loop() {
  // Wait for a TCP client from ESP32 #1:
  // Serial.println("A");
  WiFiClient client = TCPserver.available();
  // Serial.println("B");

  if (!client || !client.connected()) {
    client = TCPserver.available();
  }
  // Serial.println("C");

  if (client && client.connected() && client.available()) {
  Serial.println("D");
    char command = client.read();
    Serial.print("ESP32 #2: - Received command: ");
    Serial.println(command);
    Serial.println(buzzerOn);

    if (command == '1') {
      buzzerOn = true;
      starttid = millis();
      target_tid = starttid;
      state = 0;
    }
    // if (command == '0') {
    //   noTone(BUZZ_PIN);
    //   buzzerOn = false;
    // }
    
  }
  if (millis() - starttid > 10000) {
      buzzerOn = false;
      noTone(BUZZ_PIN);
  }
  if (buzzerOn) {
      // if (millis()-tid > target_tid)
      if (state == 0 && millis() > target_tid) {
        state = 100;
        target_tid = millis()+2000;
        tone(BUZZ_PIN, 500);
      }
      if (state == 100 && millis() > target_tid) {
        state = 1;
        target_tid = millis()+2000;
        noTone(BUZZ_PIN);
      }
      if (state == 1 && millis() > target_tid) {
        state = 2;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, HIGH);
      }
      if (state == 2 && millis() > target_tid) {
        state = 4;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, LOW);
      }
      if (state == 4 && millis() > target_tid) {
        state = 5;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, HIGH);
      }
      if (state == 5 && millis() > target_tid) {
        state = 6;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, LOW);
      }
      if (state == 4 && millis() > target_tid) {
        state = 5;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, HIGH);
      }
      if (state == 6 && millis() > target_tid) {
        state = 7;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, LOW);
      }
      if (state == 7 && millis() > target_tid) {
        state = 8;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, HIGH);
      }
      if (state == 8 && millis() > target_tid) {
        state = 3;
        target_tid = millis()+100;
        digitalWrite(LED_PIN, LOW);
      }

      if (state == 3 && millis() > target_tid) {
        state = 0;
        target_tid = millis();
      }
    }
  client.stop();
}
