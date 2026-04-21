/****************************************************
 *  Stepper Motor Lab – Manual & Auto Control System
 *  Buttons: Mode, Start/Stop, Direction, Speed Up/Down
 *  7-Segment Display: Shows system mode/status
 *  Uses Port Manipulation for fast performance
 ****************************************************/

// ===== Buttons on D8..D12 → PORTB bits 0..4 =====
#define BTN_MODE        0      // Manual / Auto switch
#define BTN_START_STOP  1      // Start/Stop motor or step once
#define BTN_DIR         2      // Change motor direction
#define BTN_SPEED_UP    3      // Increase motor speed
#define BTN_SPEED_DOWN  4      // Decrease motor speed

// ===== Lookup table for 7-segment on PORTD (D0..D6) =====
byte digits[7] = {
  0b01111111, // 0
  0b00000110, // 1
  0b10110110, // 2
  0b10011110, // 3
  0b11001100, // 4
  0b11011010, // E
  0b00000000  // off
};

// ===== Full-step and Half-step sequences (Stepper coils) =====
// Lower 4 bits → coils signals (PC0..PC3)
// Bit4 is direction indicator LED if needed
uint8_t stepSequence[4] = {
  0b00010001,
  0b00010010,
  0b00010100,
  0b00011000
};

uint8_t HALFstepSequence[8] = {
  0b00010001,
  0b00010011,
  0b00010010,
  0b00010110,
  0b00010100,
  0b00011100,
  0b00011000,
  0b00011001
};

// ===== System state variables =====
uint8_t mode = 0;          // 0=Manual, 1=Auto
uint8_t direction = 1;     // 1=Forward, 0=Backward
uint8_t running = 0;       // Auto mode running/stop
uint8_t FULL_STEP = 0;     // 0=Half step, 1=Full step
unsigned long delayy = 200;// Step delay (speed control)
int emer = 0;              // Emergency flag
int n = 0, m = 0;          // Counters for long press timing

// ===== Display number on the 7-segment =====
void display(int n){
  PORTD = digits[n];
}

// ===== Stop motor (all coils OFF) =====
void stopStepper(){
  PORTC &= 0b00000000;
}

// ===== Forward rotation - Full step =====
void rotateF(unsigned long t){
  static int s = 0;
  PORTC = stepSequence[s] | (direction<<4);
  s++; if(s > 3) s = 0;
  delay(t);
}

// ===== Forward rotation - Half step =====
void halfRotateF(unsigned long t){
  static int s = 0;
  PORTC = HALFstepSequence[s] | (direction<<4);
  s++; if(s > 7) s = 0;
  delay(t);
}

// ===== Backward rotation - Full step =====
void rotateB(unsigned long t){
  static int s = 0;
  PORTC = stepSequence[s];
  s--; if(s < 0) s = 3;
  delay(t);
}

// ===== Backward rotation - Half step =====
void halfRotateB(unsigned long t){
  static int s = 0;
  PORTC = HALFstepSequence[s];
  s--; if(s < 0) s = 7;
  delay(t);
}

// ===== Emergency mode blinking E until Start long-press release =====
void emergencySTATE(){
  stopStepper();
  while(1){
    display(5); delay(300); // Show "E"
    display(6); delay(300); // Turn off display

    // detect long release to exit emergency
    while(!(PINB & (1<<BTN_START_STOP))){
      delay(5);
      n += 5;
      if(n >= 400) return; // leave emergency after ~2s
    }
    n = 0;
  }
}

// ===== I/O configuration =====
void setup(){
  DDRD = 0b01111111;     // 7-seg outputs D0..D6
  DDRB &= 0b11100000;    // Buttons inputs PB0..PB4
  PORTB |= 0b00011111;   // Enable pull-up resistors

  DDRC = 0b00111111;     // PC0..PC5 outputs (motor + LEDs)
  stopStepper();
  display(0);
}

// ===== Main loop =====
void loop(){

  // ===== MODE Button: toggle Manual/Auto =====
  if(!(PINB & (1<<BTN_MODE))){
    if(mode == 0){ mode = 1; display(FULL_STEP?4:3); }
    else        { mode = 0; display(FULL_STEP?2:1); stopStepper(); }
    while(!(PINB & (1<<BTN_MODE))); // wait release
  }

  // ===== START/STOP button =====
  if(!(PINB & (1<<BTN_START_STOP))){
    
    if(mode == 0){ // Manual mode: one step per press
      display(FULL_STEP?2:1);
      if(FULL_STEP){
        if(direction) rotateF(delayy); else rotateB(delayy);
      } else {
        if(direction) halfRotateF(delayy); else halfRotateB(delayy);
      }
    }
    else{ // Auto mode: toggle running
      display(FULL_STEP?4:3);
      running ^= 1;
      if(!running) stopStepper();
    }

    // Long press detection => emergency
    while(!(PINB&(1<<BTN_START_STOP))){
      delay(5); m += 5;
      if(m >= 400){ emer = 1; emergencySTATE(); }
    }
    m = 0;
  }

  // ===== Direction change =====
  if(!(PINB & (1<<BTN_DIR))){
    direction ^= 1;
    while(!(PINB & (1<<BTN_DIR))); // wait release
  }

  // ===== Speed control =====
  if(!(PINB & (1<<BTN_SPEED_UP)) && delayy > 40) delayy -= 20;
  if(!(PINB & (1<<BTN_SPEED_DOWN)) && mode == 1) delayy += 20;

  // ===== Auto continuous run =====
  if(mode == 1 && running){
    if(direction){
      if(FULL_STEP) rotateF(delayy); else halfRotateF(delayy);
    } else {
      if(FULL_STEP) rotateB(delayy); else halfRotateB(delayy);
    }
  }
}