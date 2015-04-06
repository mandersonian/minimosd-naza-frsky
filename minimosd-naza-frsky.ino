#define RX_CHANNELS 2
uint8_t PWM_PINS[RX_CHANNELS] = {3, 4};

uint32_t pitch;
uint32_t roll;

#define PWM_MIN 900
#define PWM_MAX 2250
#define PWM_RANGE 1500
#define SLIDER_WIDTH 20

char range_slider_pitch[22];
char range_slider_roll[22];

volatile uint16_t RX[RX_CHANNELS] = {1000, 1000};
volatile uint32_t PWM_time[RX_CHANNELS] = {0, 0};

volatile uint16_t RX_failsafeStatus;
volatile uint8_t RX_signalReceived = 0;

bool failsafeEnabled = false;

void readPWM(uint8_t channel) {
    uint32_t now = PIT_CVAL0; // Current counter value
    uint32_t delta = PWM_time[channel] - now; // Delta (calculated in reverse, because PIT is downcounting timer)
    
    // All of the number below are scaled to use with Teensy 3.0 running at 48Mhz
    if (delta < 120000 && delta > 43200) { // This is a valid pulse
        RX[channel] = delta / 48;
        //RX[channel] = delta / 72;
        
        // Bring failsafe status flag for current channel down every time we accept a valid signal
        RX_failsafeStatus &= ~(1 << channel);
    } else { // Beginning of the pulse
        PWM_time[channel] = now;
    }
}

// ISRs
void PWM_ISR_0() {
    readPWM(0);
}
void PWM_ISR_1() {
    readPWM(1);
}


void (*PWM_Handlers [])(void) = {
    PWM_ISR_0, 
    PWM_ISR_1
};

void initializePWMCapture() {
    // initialize PIT timer (teensy running at 48000000)
    SIM_SCGC6 |=  SIM_SCGC6_PIT;
    PIT_MCR = 0x00;          // Turn on PIT
    PIT_LDVAL0 = 0xFFFFFFFF; // Load initial value of 4294967295
    PIT_TCTRL0 = 0x01;       // Start the counter
    
    for (uint8_t i = 0; i < RX_CHANNELS; i++) {
        pinMode(PWM_PINS[i], INPUT);
        attachInterrupt(PWM_PINS[i], PWM_Handlers[i], CHANGE);
    }
}

void RX_failSafe() {
    if (RX_failsafeStatus > 0) {
        RX_signalReceived++; // if this flag reaches 10, an auto-descent routine will be triggered.
    } else {
        // Raise the FLAGS
        RX_failsafeStatus |= (1 << RX_CHANNELS) - 1;
        
        // Reset the counter
        RX_signalReceived = 0;
    }
    
    if (RX_signalReceived > 10) {
        RX_signalReceived = 10; // don't let the variable overflow
        failsafeEnabled = true; // this ensures that failsafe will operate in attitude mode
 
    } else {
        failsafeEnabled = false;
    }
}

int led = 13;

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  initializePWMCapture();
  Serial.println("SETUP DONE.");
}


void loop() {
  pitch = RX[0];
  roll = RX[1];
  
  RX_failSafe();
  if(failsafeEnabled) Serial.println("RX FAILSAFE!!!");
  
  Serial.print("PITCH: [");
  for(int i = 0; i <= SLIDER_WIDTH*(pitch-PWM_MIN)/(PWM_RANGE); i++) {
    Serial.print("=");
  }
  for(int i = 0; i < SLIDER_WIDTH*(PWM_MAX-pitch)/(PWM_RANGE); i++){
    Serial.print(" ");
  }
  Serial.print("]");
  Serial.println(pitch);
  
  Serial.print("ROLL : [");
  for(int i = 0; i <= SLIDER_WIDTH*(roll-PWM_MIN)/(PWM_RANGE); i++) {
    Serial.print("=");
  }
  for(int i = 0; i < SLIDER_WIDTH*(PWM_MAX-roll)/(PWM_RANGE); i++){
    Serial.print(" ");
  }
  Serial.print("]");
  Serial.println(roll);
  
  Serial.println();
  
  
  digitalWrite(led, HIGH);
  delay(200);
  digitalWrite(led, LOW);
  delay(200);
}
