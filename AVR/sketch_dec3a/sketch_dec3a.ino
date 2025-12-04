// Buttons on D8..D12 => PORTB bits 0..4
#define BTN_MODE        0
#define BTN_START_STOP  1
#define BTN_DIR         2
#define BTN_SPEED_UP    3
#define BTN_SPEED_DOWN  4

// Seven segment on D0..D6 => PORTD bits 0..6
byte digits[7] = {
  0b01111111,
  0b00000110,
  0b10110110,
  0b10011110,
  0b11001100,
  0b11011010,
  0b00000000
};

// Step sequences (same as doctor)
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

uint8_t mode = 0;
uint8_t direction = 1;
uint8_t running = 0;
uint8_t FULL_STEP = 0;
unsigned long delayy = 200;
int emer = 0;
int n = 0, m = 0;

void display(int n){
  PORTD = digits[n];
}

void stopStepper(){
  PORTC &= 0b00000000;
}

void rotateF(unsigned long t){
  static int s=0;
  PORTC = stepSequence[s] | (direction<<4);
  s++; if(s>3) s=0;
  delay(t);
}

void halfRotateF(unsigned long t){
  static int s=0;
  PORTC = HALFstepSequence[s] | (direction<<4);
  s++; if(s>7) s=0;
  delay(t);
}

void rotateB(unsigned long t){
  static int s=0;
  PORTC = stepSequence[s];
  s--; if(s<0) s=3;
  delay(t);
}

void halfRotateB(unsigned long t){
  static int s=0;
  PORTC = HALFstepSequence[s];
  s--; if(s<0) s=7;
  delay(t);
}

void emergencySTATE(){
  stopStepper();
  while(1){
    display(5);
    delay(300);
    display(6);
    delay(300);

    while(!(PINB & (1<<BTN_START_STOP))){
      delay(5);
      n+=5;
      if(n>=400) return;
    }
    n=0;
  }
}

void setup(){
  DDRD = 0b01111111;
  DDRB &= 0b11100000;
  PORTB |= 0b00011111;

  DDRC = 0b00111111;

  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);

  display(0);
}

void loop(){

  if(!(PINB & (1<<BTN_MODE))){
    if(mode==0){ mode=1; display(FULL_STEP?4:3); }
    else{ mode=0; display(FULL_STEP?2:1); stopStepper(); }
    while(!(PINB&(1<<BTN_MODE)));
  }

  if(!(PINB & (1<<BTN_START_STOP))){
    if(mode==0){
      display(FULL_STEP?2:1);
      if(FULL_STEP){
        if(direction) rotateF(delayy); else rotateB(delayy);
      } else {
        if(direction) halfRotateF(delayy); else halfRotateB(delayy);
      }
    }
    else{
      display(FULL_STEP?4:3);
      running ^= 1;
      if(!running) stopStepper();
    }

    while(!(PINB&(1<<BTN_START_STOP))){
      delay(5); m+=5;
      if(m>=400){ emer=1; emergencySTATE(); }
    }
    m=0;
  }

  if(!(PINB & (1<<BTN_DIR))){
    direction ^= 1;
    while(!(PINB&(1<<BTN_DIR)));
  }

  if(!(PINB & (1<<BTN_SPEED_UP))){
    if(delayy>40) delayy -= 20;
  }

  if(!(PINB & (1<<BTN_SPEED_DOWN)) && mode==1){
    delayy += 20;
  }

  if(mode==1 && running){
    if(direction){
      if(FULL_STEP) rotateF(delayy); else halfRotateF(delayy);
    } else {
      if(FULL_STEP) rotateB(delayy); else halfRotateB(delayy);
    }
  }
}
