/*
That LCD works only for LCD-MODULE 1x8.

The circuit:

- LCD RS pin to digital pin 2
- LCD EN pin to digital pin 8
- LCD D4 pin to digital pin 4
- LCD D5 pin to digital pin 5
- LCD D6 pin to digital pin 6
- LCD D7 pin to digital pin 7
- LCD R/W pin to digital pin 3
- LCD VSS pin to ground
- LCD VDD pin to +5V

Program made by Drejc Marolt.
*/

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7, rw=3;
// variable empty is turned on
//bool empty = true;
// variable clear is turned on
int clear = 1;

void setup() {
  // initialize the serial communications (for example 9600)
  Serial.begin(9600);
  // initialize the LCD
  lcdInit();
  // call instruction "Display ON"
  instruction(0x0c);
  // wait for busy()
  while (busy() & 0x80);
  // display "B" to the LCD
  //writeData(0x42);
  // wait for busy()
  //while (busy() & 0x80);
  // display "C" to the LCD
  //writeData(0x41);
  // wait for busy()
  //while (busy() & 0x80);
  // display "E" to the LCD
  //writeData(0x45);
  // wait for busy()
  //while (busy() & 0x80);
}

void loop() {
  /*
  int b;
  // when characters are available in serial
  if (Serial.available()) {
    // if empty is turned on
    if (empty) {
      // then we call instruction "Clear Display"
      instruction(0x01);
      // wait for busy()
      while ((b=busy()) & 0x80);
      // then empty turns off
      empty = false;
    }

    // checks, if there is each character in serial
    while (Serial.available() > 0) {
      // for each character goes in character "byte"
      int byte = Serial.read();
      Serial.print((char)byte);
      // checks, if byte isn't 10 (new line)
      if (byte != 10) {
        // if it is not, then display each character to the LCD
        writeData(byte);
      } else {
        // if it is, then empty turns on
        empty = true;
      }
    }
  }
  */

  // checks, if there is each character in serial
  while (Serial.available() > 0) {
    // for each character goes in character "byte"
    char byte = Serial.read();
    // checks, if character is "new line" or "return"
    if (byte == '\n' | byte == '\r') {
      // if it is, then clear turns on
      clear = 1;
    } else {
      // if it is not, then checks, if clear is turned on
      if (clear == 1) {
        // if it is, then we call instruction "Clear Display"
        instruction(0x01);
        // wait for busy()
        while (busy() & 0x80);
        // then clear turns off
        clear = 0;
      }
      // display each character to the LCD
      writeData(byte);
    }
  }
}

void lcdInit() {

  pinMode(rs, OUTPUT); // Sets the pin RS as OUTPUT
  pinMode(en, OUTPUT); // Sets the pin Enable as OUTPUT
  pinMode(d4, OUTPUT); // Sets the pin D4 as OUTPUT
  pinMode(d5, OUTPUT); // Sets the pin D5 as OUTPUT
  pinMode(d6, OUTPUT); // Sets the pin D6 as OUTPUT
  pinMode(d7, OUTPUT); // Sets the pin D7 as OUTPUT
  pinMode(rw, OUTPUT); // Sets the pin R/W as OUTPUT



  // wait for 15 ms
  delay(15);
  
  digitalWrite(rs, LOW); // Sets the pin RS off
  digitalWrite(rw, LOW); // Sets the pin R/W off
  set_nimble(3); // Sets nimble in "000011"
  //wait for 5 ms
  delay(5);
  set_nimble(3); // Sets nimble in "000011"
  // wait for 1 ms
  //delay(1);
  // wait for 100 ns
  delayMicroseconds(100);
  set_nimble(3); // Sets nimble in "000011"
  // wait for 1 ms
  delay(1);
  set_nimble(2); // Sets nimble in "000010"
  while (busy() & 0x80); // wait for busy()
  // call the instruction "Function Set"
  instruction(0x20);
  // wait for busy()
  while (busy() & 0x80);
  // call the instruction "Display OFF"
  instruction(0x08);
  // wait for busy()
  while (busy() & 0x80);
  // call the instruction "Clear Display"
  instruction(0x01);
  // wait for busy()
  while (busy() & 0x80);
  // call the instruction "Entry Mode Set"
  instruction(0x06);
  // wait for busy()
  while (busy() & 0x80);
  // call the instruction "Set DDRAM address"
  /*instruction(0x80);
  // wait for busy()
  while (busy() & 0x80);
  */
}

void instruction(char inst) {
  digitalWrite(rs, LOW); // Sets the pin RS off
  if (digitalRead(rw)) { // checks, if function reads R/W
    // if yes, then we change modes of digital pins
    pinMode(d4, OUTPUT); // Sets the pin D4 as OUTPUT
    pinMode(d5, OUTPUT); // Sets the pin D5 as OUTPUT
    pinMode(d6, OUTPUT); // Sets the pin D6 as OUTPUT
    pinMode(d7, OUTPUT); // Sets the pin D7 as OUTPUT
  }
  digitalWrite(rw, LOW); // Sets the pin R/W off
  set_nimble(inst>>4); // Sets nimble for inst in shift right by 4
  set_nimble(inst); // Sets nimble for inst
}

void set_nimble(char nimble) {
  int gpio; // initialize integer gpio
  // initialize gpio for d4 (4); gpio is less than d7 (7) or equal to d7 (7)
  // increase gpio for 1
  for (gpio = d4; gpio <= d7; gpio++) {
    digitalWrite(gpio, nimble & 1);  // Sets the pin gpio ON (it sets in "nimble AND 1")
    nimble >>= 1; // shift nimble right by 4 
  }
  digitalWrite(en, HIGH); // Sets the pin Enable on.
  // wait for 1 ns
  delayMicroseconds(1);
  digitalWrite(en, LOW); // Sets the pin Enable off.
  // wait for 1 ns
  delayMicroseconds(1);
}

char busy() {
  digitalWrite(rs, LOW); // Sets the pin RS off.
  if (!digitalRead(rw)) { // checks, if function doesn't read R/W
    // if not, then we change modes of digital pins
    pinMode(d4, INPUT); // Sets the pin D4 as INPUT
    pinMode(d5, INPUT); // Sets the pin D5 as INPUT
    pinMode(d6, INPUT); // Sets the pin D6 as INPUT
    pinMode(d7, INPUT); // Sets the pin D7 as INPUT
  }
  digitalWrite(rw, HIGH); // Sets the pin R/W on.
  char code = (get_nimble()<<4) | get_nimble(); // Initialize character code. Character code will get
  // nimble by shift left for 4 or get nimble only.
  return code; // Function returns character code.
}

char get_nimble() {
  char nimble; // Initialize character nimble.
  int gpio; // Initialize integer gpio.
  digitalWrite(en, HIGH); // Sets the pin Enable on.
  // wait for 1 ns
  delayMicroseconds(1);
  nimble = 0; // Sets nimble to 0.
  // initialize gpio for d7 (7); gpio is greater than d4 (4) or equal to d4 (4)
  // decrease gpio for 1
  for (gpio = d7; gpio >= d4; gpio--) {
    nimble <<= 1; // shift nimble left by 1
    nimble |= digitalRead(gpio); // in variable nimble add bitwise OR from function, that reads gpio
  }
  digitalWrite(en, LOW); // Sets the pin Enable off.
  // wait for 1 ns
  delayMicroseconds(1);
  return nimble; // Function returns character nimble.
}

void writeData(char data) {
  digitalWrite(rs, HIGH); // Sets the pin RS on.
  if (digitalRead(rw)) { // checks, if function reads R/W
    // if yes, then we change modes of digital pins
    pinMode(d4, OUTPUT); // Sets the pin D4 as OUTPUT
    pinMode(d5, OUTPUT); // Sets the pin D5 as OUTPUT
    pinMode(d6, OUTPUT); // Sets the pin D6 as OUTPUT
    pinMode(d7, OUTPUT); // Sets the pin D7 as OUTPUT
  }
  digitalWrite(rw, LOW); // Sets the pin R/W off.
  set_nimble(data>>4); // Sets nimble for data in shift right by 4
  set_nimble(data); // Sets nimble for data
  // wait for busy()
  while (busy() & 0x80);
}

char readData() {
  
}
