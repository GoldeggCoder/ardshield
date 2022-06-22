/*
 * Author:  Benedikt Walter ft. Joachim Schoderböck
 * Datum:   15.06.2022
 * Projekt: LEDs an 8 Bit I/O Expander
 * Verwendung: look at README.md
 */

#include <Wire.h>

#define RGB_R 0x04                          //Port B (D10)                Diese defines werden verwendet um die PINs festzulegen
#define RGB_G 0x02                          //Port B (D9)
#define RGB_B 0x20                          //Port D (D5)
#define POTI ~0x01                          //Port C (A0)
#define TAST1 ~0x80                         //Port D (D7)
#define TAST2 ~0x40                         //Port D (D6)
#define LED1 0x01                           //Das sind keine PINs...das ist nur das Raster für die Übertragung über Wire 
#define LED2 0x02
#define LED3 0x04

#define PINS_PORTD RGB_B & TAST1 & TAST2    //hier werden die LEDs, POTI, TASTER und RGB zusammengelegt und in ein Hexadezimalmuster
#define PINS_PORTB RGB_R | RGB_G            //geschrieben
#define PINS_PORTC POTI

#define DEBOUNCE_TIME 200

const int RGB_PINS[] = {10, 9, 5}, POTI_PIN = A0;
int currentPick = 0, RGB_values[] = {0,0,0};
unsigned long long lasttime = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  DDRD = PINS_PORTD;                        //Ports werden festgelegt(0...input, 1...output)
  DDRB = PINS_PORTB;
  DDRC = PINS_PORTC;
  PORTD = (~TAST1) | (~TAST2);              //PULLUP Widerstände werden festgelegt
}

void loop() {
  RGB_values[currentPick] = analogRead(POTI_PIN) / 4;   //analog Werte werden eingelesen und direkt in die RGB values reingeschrieben
  if((millis() - lasttime > DEBOUNCE_TIME) && ((PIND & ~TAST1) == 0) && (currentPick > 0)) {
    currentPick--;                                      //TAST1 wird überprüft
    lasttime = millis();
  }
  else if((millis() - lasttime > DEBOUNCE_TIME) && ((PIND & ~TAST2) == 0) && (currentPick < 2)) {
    currentPick++;                                      //TAST2 wird überprüft
    lasttime = millis();
  }
  for(int i = 0; i < 3; i++)
    analogWrite(RGB_PINS[i], RGB_values[i]);            //RGB wird ausgegeben
  Wire.beginTransmission(0x20);
  switch(currentPick)                                   //LEDs werden ausgegeben
  {
    case 0: Wire.write(~LED1); break;
    case 1: Wire.write(~LED2); break;
    case 2: Wire.write(~LED3); break;
    default: break;
  }
  Wire.endTransmission();
}
