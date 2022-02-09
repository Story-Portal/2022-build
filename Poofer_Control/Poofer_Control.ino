/* 
 * Poofer_Control - Flame effect sequencer for Story Portal
 * Arduino Mega 2560
 * 
 * 20220118  MJB  Initial Rev.  (Michael J Bakula)
 * 20220123  MJB  Pin assignments
 * 20220129  MJB  Reassigned pins for interrupt-driven design
 * 
 */

/* Global Variables */

enum ModeList {
  STANDBY,
  MANUAL,
  SHOW,
  BEAT,
  W,            // unused
  X,            //
  Y,            //
  Z             //
};

volatile byte Mode = 0;  // initialize to STANDBY

/* Interrupt Service Routines */

// External

//   Mode switch
ISR(PCINT0_vect){
  Mode = PINB;             // update Mode
}

//   Manual buttons
//   Hall sensors
//   Beat button

// Timers

//   Start timer
//   Stop timer
//   Beat timer


/* Main */

int main() {
  // set up solenoid outputs
  DDRA = 0xFF;                        // all pins to OUTPUT

  // set up auxiliary outputs
  DDRC = _BV(PC1)|_BV(PC0);           // wheel state output (1), button light output (0)

  // set up sensors
  DDRD = _BV(PD2)|_BV(PD1)|_BV(PD0);     // set button(2), hall sensor(1), hall sensor(0) to input
  PIND = _BV(PD2)|_BV(PD1)|_BV(PD0);     // set pull ups
  EIMSK = _BV(PD2)|_BV(PD1);            // select pins for external interrupts
  //  PD0 is reserved for a future sensor  (MJB)

  // set up manual switches
                                      // DDRK defaults to input
  PINK = 0x7F;                        // 7 pull up resistors
  PCMSK2 = 0x7F;                      // select 7 pins for PCIE2                        
  
  // set up the mode select switch
                                      //  DDRB defaults to input
  PINB = _BV(PB2)|_BV(PB1)|_BV(PB0);  // set pull-ups
  PCMSK0 = _BV(PCIE0);                // enable pin change interrupt 0
  
  // Mode loop
  for(;;){
    
    switch (ModeList(Mode)) {
      case STANDBY:
        break;
      case MANUAL:
        break;
      case SHOW:
        break;
      case BEAT:
        break;
      default:
        Mode = STANDBY;
        break;
    }
  }
}
