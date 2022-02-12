const bool AUDIO_ONLY = false;

ISR(PCINT2_vect) {
  if (!AUDIO_ONLY) {
    PORTA = PINK;
  }

  PORTC |= _BV(PC0);
}

int main() {
  cli();
  DDRA = 0xFF;  // set port A as output
  // ===================================== MANUAL BUTTON TEST
  DDRK = 0x00;          // set port K as input for
  PCMSK2 = 0b01111111;  // select 7 pins for PCIE2
  PORTK = 0b01111111;   // 7 pull up resistors
  PORTA = 0b01111111;
  PCICR |= _BV(PCIE2);
  //  PCICR &= ~(_BV(PCIE2));

  // ===================================== /end

  // ========== Mode Change Test
  // === set up the mode select switch
  //  DDRB defaults to input
  //  DDRB = 0x00; // set port B as input just in case
  //  // enable pin change interrupt 0
  //  PCICR |= _BV(PCIE0);
  //  PCMSK0 = 0b00001111;
  //  // set pull-ups
  //  PORTB = 0b00001111;
  // === /mode select
  sei();
  while (true) {
  }
}
