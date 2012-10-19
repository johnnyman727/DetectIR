//IR Detector
//by Jialiya Huang and Jon McKay

int original_id = 12345;
int writing_id = original_id;


const int startHighMax = 32;
int startHighCount = 0;
const int startLowMax = 16;
int startLowCount = 0;

int signalRepeat = 3;
int currentSignalLength = 0;
void setup(){
  
  Serial.begin(9600);
  
  pinMode(A0,OUTPUT);//led  pin
  
  cli();//diable interrupts
  
  //set up continuous sampling of analog pin 0 at 38.5kHz
 
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS1) | (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 64 prescaler- 16mHz/32=500kHz
  Serial.println( 1 << ADPS2);
  Serial.println(ADPS0);
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts
  
  analogWrite(A0, 0);
}

ISR(ADC_vect) {//when new ADC value ready
  
  // If the max detector count for highs isn't hit yet
  // and we haven't started writing out the id
  if ((writing_id == original_id) && (startHighCount < startHighMax)) {
    // Send out a high
    analogWrite(A0, 255);
    startHighCount++;
  }
    // If the max detector count fow lows isn't hit yet
    // and we have finished with the highs
  // and we haven't started writing out the id
  else if ((writing_id == original_id) && (startLowCount < startLowMax)) {
    analogWrite(A0, 0);
    startLowCount++;
  }
  // If the highs and lows are done, start writing the signal
  else {
    // If we still have a writing id
    if (writing_id) {
      
      // If the LSB is 1
      if (writing_id & 1) {
        // Send a high
        
        analogWrite(A0, 255);
      } else {
        // Else send a low
        analogWrite(A0, 0);
    }
    currentSignalLength++;
    
    // If we have sent the signal the correct amount of times
    if (currentSignalLength == signalRepeat) {
        // Shift the next least significant bit
        writing_id >>= 1;
        currentSignalLength = 0;
    }
    }
  }
  
  // If we have written the whole number
  if (!writing_id) {
    
    // Reset all our vars
    writing_id = original_id;
    startHighCount = 0;
    startLowCount = 0;
    currentSignalLength = 0;
  }

}


void loop(){
//  analogWrite(A0, 255);
//
//  delay(1000);
//  analogWrite(A0, 0);
//  delay(1000); 
}




