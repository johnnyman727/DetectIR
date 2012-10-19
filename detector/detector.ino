//IR Detector
//by Jialiya Huang and Jon McKay
//http://www.instructables.com/id/Arduino-Timer-Interrupts/
//http://www.instructables.com/id/Arduino-Timer-Interrupts/


byte high = 67;
byte low = 248;
byte currentVal = 0;

const int startHighMax = 32;
int startHighCount = 0;
const int startLowMax = 16;
int startLowCount = 0;

boolean isSignal = false;

int signalRepeat = 3;
int currentSignalLength = 0;
int currentSignal = 0;
const int maxUserIdLength = 16;
int userId[maxUserIdLength];
int currentUserIdPos = 0;

void setup(){
  
  Serial.begin(9600);
  
  pinMode(13,OUTPUT);//led indicator pin
  pinMode(12,OUTPUT);//output pin
  
  cli();//diable interrupts
  
  //set up continuous sampling of analog pin 0 at 38.5kHz
 
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS1) |(1 << ADPS2) | (1 << ADPS0); //set ADC clock with 64 prescaler- 16mHz/32=500kHz
//  ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts
}


ISR(ADC_vect) {//when new ADC value ready
  
  if (!isSignal) {
    
      if (ADCH != low && (startHighCount < startHighMax)) {
         startHighCount++;
      }
      
      else if (startLowCount < startLowMax){
        startLowCount++;
      }
      
      if (startLowCount == startLowMax && startHighCount == startHighMax) {
        isSignal = true;
        startLowCount = startHighCount = 0;
      }
  }
  else {
      currentVal = (ADCH == low ? 0 : 1);
      currentSignal += currentVal;
      
      if (currentVal < signalRepeat) {
         currentSignalLength++; 
      }
      
      if (currentSignalLength >= signalRepeat) {
        userId[currentUserIdPos] = ((float)signalRepeat/(float)currentSignal >  (float)(signalRepeat/2) ? 1 : 0);
        currentUserIdPos++;
        currentSignal = 0;
        currentSignalLength = 0;
      }
      
      if (currentUserIdPos == (maxUserIdLength - 1)) {
         currentUserIdPos = 0;
        isSignal = false; 
      }
  }
}


void loop(){
//  Serial.println("Analog 0: " + analogRead(A0));
//    Serial.println(startHighCount);
  for (int i = 0; i < maxUserIdLength; i++) {
    Serial.print(userId[i]);
  }
  
  Serial.println(" Done");

}




