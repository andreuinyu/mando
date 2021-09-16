#include <Joystick.h>
#include <Wire.h>

Joystick_ Joystick;

#define addr 0x27 //I2C address for the GPIO expansion IC

#define DEBUG 1
#define VERBOSE 0
#if DEBUG == 1
void printBinary(int v, int num_places) {
  int mask = 0, n;
  for (n = 1; n <= num_places; n++) {mask = (mask << 1) | 0x0001;}
  v = v & mask;  // truncate v to specified number of places
  while (num_places) {
    if (v & (0x0001 << (num_places - 1))) {Serial.print("1");}
    else {Serial.print("0");}
    --num_places;
    if (((num_places % 4) == 0) && (num_places != 0)) {Serial.print(" ");}
  }
  Serial.println("");
}
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#define debugbin(x) printBinary(x, 8)

#else
#define debug(x)
#define debugln(x)
#define debugbin(x)
#endif

//Pins
#define Inter 0
#define Trigger 14
#define Up 1
#define Down 16
#define Left 5
#define Right 7
#define Ok 15

int Pitch;
int Roll;
int Yaw;
int Throttle;

int EIO0;
int EIO1;
int EIO2;
int EIO3;
int EIO4;
int EIO5;
int EIO6;
int EIO7;

int currentTrigger;
int lastTrigger;
int currentUp;
int lastUp;
int currentDown;
int lastDown;
int currentLeft;
int lastLeft;
int currentRight;
int lastRight;
int currentOk;
int lastOk;
int currentEIO0;
int lastEIO0;
int currentEIO1;
int lastEIO1;
int currentEIO2;
int lastEIO2;
int currentEIO3;
int lastEIO3;
int currentEIO4;
int lastEIO4;
int currentEIO5;
int lastEIO5;
int currentEIO6;
int lastEIO6;
int currentEIO7;
int lastEIO7;

// ISR flags that detects whether a switch was pressed
volatile bool status = true;
bool ISR_change = false;

// Prevent the ISR triggering on a switch bounce
unsigned long prevMillis = millis();

void setup() {
  if (DEBUG) {
    Serial.begin(300);
  }
  pinMode(A1, INPUT_PULLUP); //Pitch
  pinMode(A2, INPUT_PULLUP); //Roll
  pinMode(A3, INPUT_PULLUP); //Yaw
  pinMode(A0, INPUT_PULLUP); //Throttle
  pinMode(Trigger, INPUT); //Trigger
  pinMode(Up, INPUT); //Up
  pinMode(Down, INPUT); //Down
  pinMode(Left, INPUT); //Left
  pinMode(Right, INPUT); //Right
  pinMode(Ok, INPUT); //Ok

  // Initialize Joystick Library
  Joystick.begin(false);
  Joystick.setXAxisRange(0, 1024);
  Joystick.setYAxisRange(0, 1024);
  Joystick.setZAxisRange(0, 1024);
  Joystick.setThrottleRange(0, 1024);

  //Initialize I2C
  Wire.begin();

  // Turn OFF all pins by sending a high byte (1 bit per byte)
  Wire.beginTransmission(addr);
  Wire.write(0b00000000);
  Wire.endTransmission();

  // Interrupt routine
  pinMode(Inter, INPUT);
  attachInterrupt(digitalPinToInterrupt(Inter), ISRoutine, FALLING);
}

void loop() {
  // Read Joystick
  Pitch = analogRead(A1);
  Roll = analogRead(A2);

  // Read Rudder Pedals
  Yaw = analogRead(A3);

  // Read Throttle
  Throttle = analogRead(A0);

  // Read main I/O Switches
  int currentTrigger = digitalRead(Trigger); // Button 0: Trigger
  if (currentTrigger != lastTrigger) {
    Joystick.setButton(0, currentTrigger);
    lastTrigger = currentTrigger;
  }

  int currentUp = digitalRead(Up); // Button 1: Up
  if (currentUp != lastUp) {
    Joystick.setButton(1, currentUp);
    lastUp = currentUp;
  }

  int currentDown = digitalRead(Down); // Button 2: Down
  if (currentDown != lastDown) {
    Joystick.setButton(2, currentDown);
    lastDown = currentDown;
  }

  int currentLeft = digitalRead(Left); // Button 3: Left
  if (currentLeft != lastLeft) {
    Joystick.setButton(3, currentLeft);
    lastLeft = currentLeft;
  }

  int currentRight = digitalRead(Right); // Button 4; Right
  if (currentRight != lastRight) {
    Joystick.setButton(4, currentRight);
    lastRight = currentRight;
  }

  int currentOk = digitalRead(Ok); // Button 5: Ok
  if (currentOk != lastOk) {
    Joystick.setButton(5, currentOk);
    lastOk = currentOk;
  }

  //Read expansion I/O switches, if there's any update
  if (ISR_change) {
    debug("\n");
    ISR_change = false;

    // Let's READ the status of the pins on the expander
    byte resposta = 0;
    Wire.requestFrom(addr, 1);
    if (Wire.available()) {
      resposta = Wire.read();
    }
    
    debugln("rebut:");
    debug(resposta);
    debug("\t");
    debugbin(resposta);

    // Determine pin values
    EIO0 = !(resposta & 0b00000001);
    EIO1 = !(resposta & 0b00000010);
    EIO2 = !(resposta & 0b00000100);
    EIO3 = !(resposta & 0b00001000);
    EIO4 = !(resposta & 0b00010000);
    EIO5 = !(resposta & 0b00100000);
    EIO6 = !(resposta & 0b01000000);
    EIO7 = !(resposta & 0b10000000);

    bool currentEIO0 = !EIO0; // Button 6: EIO0
    if (currentEIO0 != lastEIO0) {
      Joystick.setButton(6, currentEIO0);
      lastEIO0 = currentEIO0;
    }

    bool currentEIO1 = !EIO1; // Button 7: EIO1
    if (currentEIO1 != lastEIO1) {
      Joystick.setButton(7, currentEIO1);
      lastEIO1 = currentEIO1;
    }

    bool currentEIO2 = !EIO2; // Button 8: EIO2
    if (currentEIO2 != lastEIO2) {
      Joystick.setButton(8, currentEIO2);
      lastEIO2 = currentEIO2;
    }

    bool currentEIO3 = !EIO3; // Button 9: EIO3
    if (currentEIO3 != lastEIO3) {
      Joystick.setButton(9, currentEIO3);
      lastEIO3 = currentEIO3;
    }

    bool currentEIO4 = !EIO4; // Button 10: EIO4
    if (currentEIO4 != lastEIO4) {
      debugln(currentEIO4);
      Joystick.setButton(10, currentEIO4);
      lastEIO4 = currentEIO4;
    }

    bool currentEIO5 = !EIO5; // Button 11: EIO5
    if (currentEIO5 != lastEIO5) {
      Joystick.setButton(11, currentEIO5);
      lastEIO5 = currentEIO5;
    }

    bool currentEIO6 = !EIO6; // Button 12: EIO6
    if (currentEIO6 != lastEIO6) {
      Joystick.setButton(12, currentEIO6);
      lastEIO6 = currentEIO6;
    }

    bool currentEIO7 = !EIO7; // Button 13: EIO7
    if (currentEIO7 != lastEIO7) {
      Joystick.setButton(13, currentEIO7);
      lastEIO7 = currentEIO7;
    }
  }

  // Output Controls
  Joystick.setXAxis(Roll);
  Joystick.setYAxis(Pitch);
  Joystick.setZAxis(Yaw);
  Joystick.setThrottle(Throttle);

  if (VERBOSE) {
    debug("0:");
    debugln(currentTrigger);
    debug("1:");
    debugln(currentUp);
    debug("2:");
    debugln(currentDown);
    debug("3:");
    debugln(currentLeft);
    debug("4:");
    debugln(currentRight);
    debug("5:");
    debugln(currentOk);
    debug("6:");
    debugln(currentEIO0);
    debug("7:");
    debugln(currentEIO1);
    debug("8:");
    debugln(currentEIO2);
    debug("9:");
    debugln(currentEIO3);
    debug("10:");
    debugln(currentEIO4);
    debug("11:");
    debugln(currentEIO5);
    debug("12:");
    debugln(currentEIO6);
    debug("13:");
    debugln(currentEIO7);
    debug("\n\n");
  }
  Joystick.sendState();
}

void ISRoutine() {
  if (millis() > prevMillis + 150) {
    ISR_change = true;
    // Update the time so that we don't re-trigger
    prevMillis = millis();
  }
}
