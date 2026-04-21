
#define LED0_PIN 8  // Pin 8: Timer0 Blink LED (PB0)
#define LED1_PIN 9  // Pin 9: Timer1 PWM Fade LED (PB1)
#define LED2_PIN 10 // Pin 10: Pin احتياطي (PB2)
#define BUTTON1_PIN 2 // Pin 2: زرار تغيير سرعة Blink (PD2)
#define BUTTON2_PIN 3 // Pin 3: زرار تغيير Brightness (PD3)

volatile byte timer0_compare_value = 50; 
volatile byte timer1_duty_cycle = 0;
volatile unsigned long last_debounce_time = 0; 
const long debounce_delay = 250;


void setup() {
    
    
    // LEDs (Port B: PB0, PB1, PB2) 
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2); 
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2)); 

    // Buttons (Port D: PD2, PD3) 
    DDRD &= ~((1 << PD2) | (1 << PD3)); 
    PORTD |= (1 << PD2) | (1 << PD3); 


    // 2. تهيئة Timer0: CTC Mode 
    TCCR0A = 0; // إطفاء مؤقت
    TCCR0B = 0;
    TCCR0A |= (1 << WGM01); // ضبط CTC Mode
    OCR0A = timer0_compare_value; // القيمة الأولية للمنبه
    TIMSK0 |= (1 << OCIE0A); // تفعيل مقاطعة المقارنة (Interrupt)
    TCCR0B |= (1 << CS02) | (1 << CS00); // تشغيل Timer0 بـPrescaler 1024


    // 3. تهيئة Timer1: Fast PWM (8-bit)
    TCCR1A = 0; // إطفاء مؤقت
    TCCR1B = 0;
    TCCR1A = (1 << WGM10); // ضبط Fast PWM Mode 
    TCCR1B = (1 << WGM12); // ضبط Fast PWM Mode
    TCCR1A |= (1 << COM1A1); // Non-inverting PWM على OC1A (PB1)
    OCR1A = timer1_duty_cycle; // القيمة الأولية للـDuty Cycle (0)
    TCCR1B |= (1 << CS11) | (1 << CS10); // تشغيل Timer1 بـPrescaler 64
    

    // 4. تهيئة Timer2: Normal Mode للـDebouncing
    TCCR2A = 0; // إطفاء مؤقت
    TCCR2B = 0;
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // تشغيل Timer2 بـPrescaler 1024
    
    // 5. تفعيل المقاطعات العامة
    sei(); 
}


void loop() {
    
    unsigned long current_time = millis(); 

    if (current_time - last_debounce_time > debounce_delay) { 

        // Button 1: تغيير سرعة Blink (Timer0) 
      
        if ( (PIND & (1 << PD2)) == 0 ) { 
            last_debounce_time = current_time; 
            
            
            if (timer0_compare_value >= 255) {
                timer0_compare_value = 50; 
            } else {
                timer0_compare_value += 50; 
            }
            
            OCR0A = timer0_compare_value; // تحديث سجل Timer0
        }

        //  Button 2: تغيير مستوى الـBrightness (Timer1) 

        else if( (PIND & (1 << PD3)) == 0) {
            last_debounce_time = current_time; 
            
            // زيادة قيمة OCR1A لزيادة سطوع الـFade
            if (timer1_duty_cycle >= 255) {
                timer1_duty_cycle = 0; 
            } else {
                timer1_duty_cycle += 50; 
            }
            
            OCR1A = timer1_duty_cycle; // تحديث سجل Timer1 (الـDuty Cycle)
        }
    }
}


ISR(TIMER0_COMPA_vect) {
    // تقليب حالة LED0 (Pin 8 / PB0) يدوياً
    PORTB ^= (1 << PB0); 
}