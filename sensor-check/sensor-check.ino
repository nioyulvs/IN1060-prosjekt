/* Prosjekt: Fargeanimasjon med WS2812B LED-strip og infrarød sensor
 Beskrivelse: Når en sensor registrerer et objekt (f.eks. lys/mørke),
              starter en fargeanimasjon på en LED-strip i 10 sekunder.
              Animasjonen består av en bevegelig blokk som endrer farge kontinuerlig.
 Bruker: Adafruit NeoPixel bibliotek, digital inngang, millis() for timing.
 */


#include <Adafruit_NeoPixel.h> // Bibliotek for å styre WS2812B (NeoPixel) LED-striper

// LED-strip konfigurasjon
#define PIN         6           // Data-pin koblet til LED-stripen
#define NUM_LEDS    32          // Antall LEDs på stripen
#define BLOCK_SIZE  3           // Størrelse på den animerte fargeblokken

Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800); // Initialiserer stripen

// Sensor
int OUT = 7; // Digital inngang koblet til HW201 infrarød sensor

// Animasjonsparametere
int baseHue = 0;    // Basis fargetone (hue) for fargeanimasjonen
int direction = 1;  // Retning på animasjonen: 1 = fremover, -1 = bakover
int position = 0;   // Nåværende posisjon for startpunktet til fargeblokken

void setup() {
  Serial.begin(9600); // Starter seriell kommunikasjon (for debugging)
  pinMode(OUT, INPUT); // Setter sensoren som input

  strip.begin();  // Initialiserer LED-stripen
  strip.show();   // Slår av alle LEDs ved start
}

void loop() {
  int sensorValue = digitalRead(OUT); // Leser sensorens tilstand (0 = objekt registrert)

  if (sensorValue == 0) {
    Serial.println("registrert");

    startAnimation(10000); // Starter animasjon i 10 000 ms (10 sekunder)

    clearStrip(); // Slår av alle lys etter animasjonen
  } else {
    Serial.println("other colors detected"); // For debugging: sensor har ikke registrert noe
  }

  delay(500); // Venter litt før neste sensoravlesning
}

// Starter fargeanimasjonen i angitt varighet (millisekunder)
void startAnimation(unsigned long duration) {
  unsigned long endTime = millis() + duration; // Tidspunkt hvor animasjonen skal stoppe

  while (millis() < endTime) {
    updateColors();       // Oppdater basisfarge (hue)
    drawBackground();     // Tegn bakgrunnsfarge på hele stripen
    drawMovingBlock();    // Tegn animert fargeblokk oppå bakgrunnen
    strip.show();         // Oppdater stripen med endringer
    delay(70);            // Kontrollerer hvor raskt animasjonen går
    updatePosition();     // Flytt blokken til ny posisjon
  }
}

// Oppdaterer basis-hue kontinuerlig for å gi animasjonen en jevn fargesyklus
void updateColors() {
  baseHue = (baseHue + 1) % 255; // Øker fargetone, ruller over til 0 etter 254
}

// Fyller hele stripen med en svak bakgrunnsfarge
void drawBackground() {
  int backgroundHue = (baseHue - 5) * 256; // Litt forskjellig hue fra fargeblokken
  unsigned long backgroundColor = strip.ColorHSV(backgroundHue, 255, 40); // Svakere intensitet

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, backgroundColor); // Setter hver LED til bakgrunnsfargen
  }
}

// Tegner den bevegelige fargeblokken i sterkere farger
void drawMovingBlock() {
  int movingHue = baseHue * 256; // Nåværende fargetone
  unsigned long movingColor = strip.ColorHSV(movingHue, 255, 255); // Full intensitet

  // Går gjennom blokken og setter farge på de relevante LEDene
  for (int i = 0; i < BLOCK_SIZE; i++) {
    int pixelIndex = position + i; // Posisjonen til hver LED i blokken
    if (pixelIndex >= 0 && pixelIndex < NUM_LEDS) {
      strip.setPixelColor(pixelIndex, movingColor); // Setter fargen hvis indeksen er innenfor stripen
    }
  }
}

// Flytter blokken fremover eller bakover og bytter retning i kantene
void updatePosition() {
  position += direction; // Flytter blokken én posisjon i gjeldende retning

  // Sjekker om vi er ved en ende og må snu retning
  if (position <= 0 || position >= NUM_LEDS - BLOCK_SIZE) {
    direction *= -1; // Endrer bevegelsesretning (spretter tilbake)
  }
}

// Slår av alle LEDs (brukes etter animasjonen)
void clearStrip() {
  strip.clear();  // Setter alle LEDs til av (sort)
  strip.show();   // Oppdaterer stripen slik at endringen vises
}
