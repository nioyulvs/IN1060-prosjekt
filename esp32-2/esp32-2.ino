// ESP32 #3: TCP SERVER + LED + PIEZO (i esp32-1 koden = server1Adress og server1Port)
#include <WiFi.h>
#define LED_PIN 19                // LED tilkoblet GPIO19
#define BUZZ_PIN 12              // Buzzer tilkoblet GPIO12
#define SERVER_PORT 4080         // Porten som TCP-serveren lytter på (forskjellig fra esp32-3 koden)

const char* ssid = "Norinn";             // WiFi SSID
const char* password = "Kakesjokolade";  // WiFi-passord
bool buzzerOn = false;                   // Variabel for å holde styr på om buzzer skal være aktiv
long starttid = 0;                       // Starttid for buzzer-aktivitet
long target_tid = 0;                     // Tidspunkt for neste tilstandsendring
int state = 0;                           // Nåværende tilstand i tilstandsautomaten
WiFiServer TCPserver(SERVER_PORT);      // Oppretter TCP-serverobjekt

void setup() {
  Serial.begin(9600);                   // Starter seriell kommunikasjon

  pinMode(BUZZ_PIN, OUTPUT);           // Setter buzzer-pin som utgang
  digitalWrite(BUZZ_PIN, LOW);         // Slår av buzzer
  pinMode(LED_PIN, OUTPUT);            // Setter LED-pin som utgang

  Serial.println("ESP32 #2: TCP SERVER + AN LED");

  // Koble til WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);                        // Venter til tilkobling er fullført
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Skriver ut IP-adressen til ESP32 som må endres i esp32-1 koden
  Serial.print("ESP32 #2: TCP Server IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("ESP32 #2: -> Please update the serverAddress in ESP32 #1 code");

  // Starter TCP-serveren
  TCPserver.begin();
}

void loop() {
  // Venter på en TCP-klient (ESP32 #1)
  // Serial.println("A");
  WiFiClient client = TCPserver.available();  // Sjekker om en klient er tilkoblet
  // Serial.println("B");

  if (!client || !client.connected()) {       // Hvis ingen klient er tilkoblet, prøv igjen
    client = TCPserver.available();
  }
  // Serial.println("C");

  if (client && client.connected() && client.available()) {  // Hvis klient er tilgjengelig og sender data
    Serial.println("D");
    char command = client.read();            // Leser innkommende kommando
    Serial.print("ESP32 #2: - Received command: ");
    Serial.println(command);
    Serial.println(buzzerOn);

    if (command == '1') {                    // Hvis kommando er '1', aktiver buzzer
      buzzerOn = true;
      starttid = millis();                  // Noterer starttidspunkt
      target_tid = starttid;                // Setter mål for første tilstandsendring
      state = 0;                            // Starter fra første tilstand
    }
    // if (command == '0') {
    //   noTone(BUZZ_PIN);                 // (Kommentert ut) kode for å deaktivere buzzer
    //   buzzerOn = false;
    // }
  }

  if (millis() - starttid > 10000) {        // Stopper buzzer etter 10 sekunder
    buzzerOn = false;
    noTone(BUZZ_PIN);
  }

  if (buzzerOn) {
    // Tilstandsautomat for buzzer og LED
    if (state == 0 && millis() > target_tid) {
      state = 100;
      target_tid = millis() + 2000;         // Spiller lyd i 2 sekunder
      tone(BUZZ_PIN, 1000);
    }
    if (state == 100 && millis() > target_tid) {
      state = 1;
      target_tid = millis() + 2000;         // Slår av lyd i 2 sekunder
      noTone(BUZZ_PIN);
    }
    if (state == 1 && millis() > target_tid) {
      state = 2;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, HIGH);          // LED på
    }
    if (state == 2 && millis() > target_tid) {
      state = 4;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, LOW);           // LED av
    }
    if (state == 4 && millis() > target_tid) {
      state = 5;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, HIGH);          // LED på igjen
    }
    if (state == 5 && millis() > target_tid) {
      state = 6;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, LOW);           // LED av
    }
    if (state == 4 && millis() > target_tid) {
      state = 5;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, HIGH);          
                                            /* Denne delen av koden er duplisert, men
                                            vi valgte å ikke fjerne den fordi ESP32-kodene har vært ustabile 
                                            og uforutsigbare ved små endringer */
    }
    if (state == 6 && millis() > target_tid) {
      state = 7;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, LOW);
    }
    if (state == 7 && millis() > target_tid) {
      state = 8;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, HIGH);
    }
    if (state == 8 && millis() > target_tid) {
      state = 3;
      target_tid = millis() + 100;
      digitalWrite(LED_PIN, LOW);
    }
    if (state == 3 && millis() > target_tid) {
      state = 0;
      target_tid = millis();                // Går tilbake til starttilstand
    }
  }
  client.stop();                            // Lukker klientforbindelsen
}
