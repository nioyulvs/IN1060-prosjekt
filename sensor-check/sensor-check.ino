/* Prosjekt: Fargeanimasjon med WS2812B LED-strip og infrarød sensor
 Beskrivelse: Når en sensor registrerer et objekt (f.eks. lys/mørke),
              starter en fargeanimasjon på en LED-strip i 10 sekunder.
              Animasjonen består av en bevegelig blokk som endrer farge kontinuerlig.
 Bruker: Adafruit NeoPixel bibliotek, digital inngang, millis() for timing.
 */


#include <Adafruit_NeoPixel.h> //bibliotek som passer til LED stripen
//LED strip - WS2812B

// LED strip setup
#define PIN         6 //LED pin
#define NUM_LEDS    32 //definerer mengen LED på stripen som skal brukes
#define BLOCK_SIZE  3 //størrelsen på den animerte "blokken"
Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// sensor pin
// HW201
int OUT = 7;

// Animasjon parametere
int baseHue = 0;  // grunnfarge for fargenaimasjon
int direction = 1;    // retning på bevegelsen: 1 = fremover, -1 = bakover
int position = 0;     // nåværende posisjon for blokken

void setup() {
  Serial.begin(9600);
  pinMode(OUT, INPUT); 

  strip.begin(); // initialiserer LED-stripen
  strip.show(); // slår av alle LEDs på starten
}

void loop() {
  int sensorValue = digitalRead(OUT); 
  // leser av verdien fra sensoren (0 == registrert, 1 == ikke registrert)

  if (sensorValue == 0) {
    Serial.println("registrert");

    unsigned long endTime = millis() + 10000; // setter sluttidspunkt for animasjon til 10 sek fra "nå"

    while (millis() < endTime) {
      // øker fargeverdien for å få en syklus-effekt

      baseHue++;
      if (baseHue >= 255) baseHue = 0;

      // genererer liknende faregoner for bakgrunn og animasjonsblokk
      int movingHue = baseHue * 256;
      int backgroundHue = (baseHue - 5) * 256;

      // Bakgrunnsfarge: saturert, svakere farge
      unsigned long backgroundColor = strip.ColorHSV(backgroundHue, 255, 40);
      // bevegelig blokk: saturert, sterk farge
      unsigned long movingColor = strip.ColorHSV(movingHue, 255, 255);

      // fyll stripen med bakgrunnsfargen
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, backgroundColor);
      }

      // tegner fargeblokken i en annen nyanse (farge)
      for (int i = 0; i < BLOCK_SIZE; i++) {
        int pixelIndex = position + i;
        if (pixelIndex >= 0 && pixelIndex < NUM_LEDS) {
          strip.setPixelColor(pixelIndex, movingColor);
        }
      }

      strip.show(); //oppdaterer stripen med nye farger
      delay(70); // setter fart på animasjonen

      // oppdaterer posisjonen for neste frame
      position += direction;

      // hvis blokken når enden, snur den retning og går tilbake
      if (position <= 0 || position >= NUM_LEDS - BLOCK_SIZE) {
        direction *= -1; // "bounce back"
      }
    }

    // etter 10 sekunder skrus alle LEDs av
    strip.clear();
    strip.show();
  }

  if (sensorValue == 1) {
    Serial.println("other colors detected");
  }

  delay(500); //venter før neste avlesning
}