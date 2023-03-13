/* Oscilloscope
 *  by moty22.co.uk
 */ 

 unsigned char i, out[42], dh, range=0;

void setup()
{

    //baud rate=38400
  UCSR0A |= (1<<U2X0);   //fast rate
  UBRR0H = 0;
  UBRR0L = 51;
  UCSR0B |= (1<<RXEN0)|(1<<TXEN0); //rx and tx enabled
 
  //comperator
  DIDR1 |= _BV(AIN0D) |  _BV(AIN1D);  //analog inputs
    
    //ADC
  DIDR0 |= _BV(ADC0D);  //input=A0 
  ADCSRA |= _BV(ADPS2) | _BV(ADEN); //prescaler=16 250KHz, enabled
  ADMUX |= _BV(REFS0) | _BV(ADLAR);  //Vref=Vcc, left justified
//    DIDR0=1;  //input=A0
//    ADMUX=0b1100000;  //left justified, A0 input
//    ADCSRA=0b11100010;    //free running
//    ADCSRA=0b10000100;    //enabled
}

void loop()
{
   //trigger when input high
    while ( ( ACSR & (1<<ACO)) ){} // Wait for low COMPERATOR output
    while ( !( ACSR & (1<<ACO)) ){} // Wait for high COMP out
 
      //sample 42 readings
    for(i=0;i<42;++i){   
    ADCSRA |=_BV(ADSC);   //start conversion
    while ( !( ADCSRA & (1<<ADIF)) ){} // Wait for conversion to complete
    
    dh = ADCL;
    out[i]=ADCH;
    ADCSRA |=_BV(ADIF);   //reset interrupt
    if(range==1){delay(1);}  //slower sweep for low freq
    }
    
      //send 42 bytes
  for(i=0;i<42;++i){ 
    while ( !( UCSR0A & (1<<UDRE0)) ){} // Wait for empty transmit buffer
    UDR0 = out[i];  // send the data
  }
      delay(500);
      
    //check bytes received
  if( (UCSR0A & (1<<RXC0)) ) {range = UDR0 - 49;}

}
