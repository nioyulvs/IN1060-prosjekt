/*
 * ESP32 TCP-klient med to knapper
 * 
 * Denne koden lar én ESP32-enhet fungere som en TCP-klient som sender
 * signaler til to separate TCP-servere (to andre ESP32-er),
 * basert på knappetrykk. Hver knapp styrer sin egen server.
 * 
 * Bruk: kommunisere med to andre ESP32-er som signaliserer en piezo og en LED hver
 */

#include <WiFi.h>      // Inkluderer bibliotek for WiFi-funksjonalitet

// Konstanter
#define BUTTON_PIN_1 21  // GPIO21 er koblet til knapp 1
#define BUTTON_PIN_2 19  // GPIO19 er koblet til knapp 2

// WIFI-innstillinger
const char* ssid = "Norinn";  // Nettverksnavn
const char* password = "Kakesjokolade";   // Nettverkspassord

// Tidligere knappetilstand
// Disse brukes for å oppdage endringer i knappestatus (trykk og slipp)
int prevbutton1 = 0;
int prevbutton2 = 0;

// TCP-innstillinger for server 1 (tilhørende knapp 1)
const char* server1Address = "192.168.57.216"; // IP-adresse til ESP32 #2
//IP-adressen må oppdateres for hver gang
const int server1Port = 4080;  // Portnummer som ESP32 #2 lytter på

// TCP-innstillinger for server 2 (tilhørende knapp 2)
const char* server2Address = "192.168.57.216"; // IP-adresse til ESP32 #3
//IP-adressen må oppdateres for hver gang
const int server2Port = 4081;   // Portnummer som ESP32 #3 lytter på

// TCP-klientobjekter
// Disse brukes for å opprette og vedlikeholde forbindelser til serverne
WiFiClient TCPclient1;
WiFiClient TCPclient2;

void setup() {
  // Konfigurer knappene som input med intern pull-up (gir HIGH som standard)
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);

  // Start seriell kommunikasjon for debugging
  Serial.begin(9600);
  Serial.println("ESP32: TCP CLIENT + TWO BUTTONS");

  // Start WiFi-modul i stasjonsmodus (kobler seg til eksisterende nettverk)
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Vent til ESP32 er koblet til WiFi-nettverket
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Forsøk å koble til første TCP-server
  if (TCPclient1.connect(server1Address, server1Port)) {
    Serial.println("Connected to first TCP server");
  } else {
    Serial.println("Failed to connect to first TCP server");
  }

  // Forsøk å koble til andre TCP-server
  if (TCPclient2.connect(server2Address, server2Port)) {
    Serial.println("Connected to second TCP server");
  } else {
    Serial.println("Failed to connect to second TCP server");
  }
}

void loop() {
  // Sjekk forbindelser

  // Hvis forbindelsen til første server er brutt, forsøk å koble til på nytt
  if (!TCPclient1.connected()) {
    Serial.println("Disconnected from 1st server");
    TCPclient1.stop();

    if (TCPclient1.connect(server1Address, server1Port)) {
      Serial.println("Reconnected to first TCP server");
    } else {
      Serial.println("Failed to reconnect to first TCP server");
    }
  }

  // Hvis forbindelsen til andre server er brutt, forsøk å koble til på nytt
  if (!TCPclient2.connected()) {
    Serial.println("Disconnected from 2nd server");
    TCPclient2.stop();

    if (TCPclient2.connect(server2Address, server2Port)) {
      Serial.println("Reconnected to second TCP server");
    } else {
      Serial.println("Failed to reconnect to second TCP server");
    }
  }

  // Knapp 1 - lesing og kommunikasjon
  int button1 = digitalRead(BUTTON_PIN_1); // 0 = trykket (pga. INPUT_PULLUP), 1 = sluppet

  // Hvis knapp 1 ble trykket ned
  if (button1 == 0 && prevbutton1 == 1) {
    TCPclient1.write('1');       // Send "trykket" signal
    TCPclient1.flush();          // Sørg for at alt blir sendt
    Serial.println("S. 1 - Knapp trykket: sendte '1'");
    delay(50);
    TCPclient2.write('1');
    TCPclient2.flush();
  }

  // Hvis knapp 1 ble sluppet opp
  if (button1 == 1 && prevbutton1 == 0) {
    TCPclient1.write('0');       // Send "sluppet" signal
    TCPclient1.flush();
    delay(50);                   // Liten forsinkelse for å sikre signal
    TCPclient1.write('0');       // Dobbel sending for sikkerhet
    TCPclient1.flush();
    Serial.println("S. 1 - Knapp sluppet: sendte '0'");
  }

  prevbutton1 = button1; // Oppdater tilstand

  // Knapp 2 - lesing og kommunikasjon
  int button2 = digitalRead(BUTTON_PIN_2);

  if (button2 == 0 && prevbutton2 == 1) {
    TCPclient2.write('1');
    TCPclient2.flush();
    Serial.println("S. 2 - Knapp trykket: sendte '1'");
    delay(50);
    TCPclient2.write('1');
    TCPclient2.flush();
  }

  if (button2 == 1 && prevbutton2 == 0) {
    TCPclient2.write('0');
    TCPclient2.flush();
    delay(50);
    TCPclient2.write('0');
    TCPclient2.flush();
    Serial.println("S. 2 - Knapp sluppet: sendte '0'");
  }

  prevbutton2 = button2; // Oppdater tilstand
}
