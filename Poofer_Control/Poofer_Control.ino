/*
 * Poofer_Control - Flame effect sequencer for Story Portal
 * Arduino Mega 2560
 *
 * 20220118  MJB  Initial Rev.  (Michael J Bakula)
 * 20220123  MJB  Pin assignments
 * 20220129  MJB  Reassigned pins for interrupt-driven design
 *
 */

// Only for vscode intellisense
// #include "allheadersa.h"
const unsigned int TICKS_PER_SECOND = 15625;

const unsigned int WHEEL_START_GAP = TICKS_PER_SECOND / 10;
const unsigned int WHEEL_STOP_TIMEOUT = TICKS_PER_SECOND;
const unsigned int FINALE_TEMPO = TICKS_PER_SECOND / 4;

const byte POOFER_SPIN_POSES[] = {
    0b11111110,  // ---------- preventing autoformat ----------
    0b11111100,  // ---------- preventing autoformat ----------
    0b11111000,  // ---------- preventing autoformat ----------
    0b11110001,  // ---------- preventing autoformat ----------
    0b11100011,  // ---------- preventing autoformat ----------
    0b11000111,  // ---------- preventing autoformat ----------
    0b10001111,  // ---------- preventing autoformat ----------
    0b10011111,  // ---------- preventing autoformat ----------
    0b10111111,  // ---------- preventing autoformat ----------
    0b10111111,  // ---------- preventing autoformat ----------
    0b10011111,  // ---------- preventing autoformat ----------
    0b10001111,  // ---------- preventing autoformat ----------
    0b11000111,  // ---------- preventing autoformat ----------
    0b11100011,  // ---------- preventing autoformat ----------
    0b11110001,  // ---------- preventing autoformat ----------
    0b11111000,  // ---------- preventing autoformat ----------
    0b11111100,  // ---------- preventing autoformat ----------
    0b11111110   // ---------- preventing autoformat ----------
};

const byte POOFER_FINALE_POSES[] = {
    0b11110111,  // ---------- preventing autoformat ----------
    0b11101011,  // ---------- preventing autoformat ----------
    0b11011101,  // ---------- preventing autoformat ----------
    0b10111110,  // ---------- preventing autoformat ----------
    0b10011100,  // ---------- preventing autoformat ----------
    0b10001000,  // ---------- preventing autoformat ----------
    0b10000000,  // ---------- preventing autoformat ----------
};

/* Global Variables */

enum ModeList {
  STANDBY,
  MANUAL,
  SHOW,
  BEAT,
  AUDIO_SHOW,
  AUDIO_BEAT,
  X,  // unused
  Y,  //
  Z   //
};

volatile byte Mode = 0;  // initialize to STANDBY
volatile byte lastMode = 0;

bool audioOnly() {
  return Mode == ModeList::AUDIO_BEAT || Mode == ModeList::AUDIO_SHOW;
}

/* Interrupt Service Routines */

// External

// Poofer control
namespace Poofers {

unsigned int count = 0;

void resetCount() {
  count = 0;
}

void fire(volatile byte values) {
  if (!audioOnly()) {
    PORTA = values;
  }
  // enable wheel spin relay (trigger audio)
  PORTC |= _BV(PC0);
  // After firing set up timer to turn off fire?
}

void clear() {
  Poofers::fire(0xFF);
}

void spinBeat() {
  Poofers::fire(POOFER_SPIN_POSES[Poofers::count]);
  Poofers::count = (Poofers::count + 1) % sizeof(POOFER_SPIN_POSES);
}

bool finaleBeat() {
  Poofers::fire(POOFER_FINALE_POSES[Poofers::count]);
  Poofers::count += 1;
  return Poofers::count == sizeof(POOFER_FINALE_POSES);
}
};  // namespace Poofers

// ========================= READ THE FUCKIN MANUAL =====================
// Manual mode watches the manual switches and drives the respective relays for
// testing and as a low-level backup.
namespace ManualMode {
// Enable interrupts
void setup() {
  // PCIE2 represents interrupts for PCINT23:16
  // cli();
  PCICR |= _BV(PCIE2);
  // sei();
}

// Disable interrupts
void teardown() {
  // cli();
  PCICR &= ~(_BV(PCIE2));
  // alt
  // PCMSK2 = 0b00000000;
  // sei();
}
};  // namespace ManualMode

//   Manual buttons
ISR(PCINT2_vect) {
  PORTA = PINK;
  // Poofers::fire(PINK);
}

// =========================== IT'S SHOWTIME ============================

namespace Wheel {
enum State { WAITING, SPINNING, FINALE };
State currentState = Wheel::State::WAITING;
volatile uint16_t lastEvent = 0;

// timer 3 - input for hall sensor
void waitingSetup() {
  // cli();
  currentState = State::WAITING;
  TCNT3 = 0;
  TIMSK3 |= _BV(ICIE3);
  // sei();
}
void waitingTeardown() {
  TCNT3 = 0;
  TIMSK3 &= ~(_BV(ICIE3));
}

// timer 4 - output for stop timeout
void spinningSetup() {
  // cli();
  waitingTeardown();
  currentState = State::SPINNING;
  Poofers::resetCount();
  // setup stoptimer interrupt
  TIMSK4 |= _BV(OCIE4A);
  // sei();
}

void spinningTeardown() {
  TIMSK4 &= ~(_BV(OCIE4A));
}

// timer 5 - output for finale tempo
void finaleSetup() {
  // cli();
  spinningTeardown();
  currentState = State::FINALE;
  Poofers::resetCount();
  TIMSK5 |= _BV(OCIE5A);
  // sei();
}
void finaleTeardown() {
  // cli();
  TIMSK5 &= ~(_BV(OCIE5A));
  // sei();
}

}  // namespace Wheel

// ========================= GET THE SHOW ON THE ROAD =====================
/*
Show mode is the most complex, which means that show mode has a number of
states:

 - waiting-to-start
 - spinning
 - stop-timeout
 - timed sequence


*/
namespace ShowMode {
void setup() {
  EIMSK |= _BV(INT0);
  Wheel::waitingSetup();
}
void teardown() {
  EIMSK &= ~(_BV(INT0));
}
};  // namespace ShowMode

// =========================== DROP THE BEAT ==========================
// Beat runs similar to show mode, except it watches the beat button instead of
// the hall sensors.
namespace BeatMode {
void setup() {
  EIMSK |= _BV(INT2);
  Wheel::waitingSetup();
}
void teardown() {
  EIMSK &= ~(_BV(INT2));
}
};  // namespace BeatMode

// ========================= FIRE CONTROL STANDING BY =============
// Standby mode is simple, the system ignores all inputs except for the mode
// selector.
namespace StandbyMode {
void setup() {
  // ManualMode::teardown();
  // ShowMode::teardown();
  // BeatMode::teardown();
}
void teardown() {}
};  // namespace StandbyMode

// On BeatButton or HallSensor
//     trigger input capture by writing to input capture pin
ISR(INT0_vect) {
  // Interrupt pin for timer 3 aka input to determine wheel start
  switch (Wheel::currentState) {
    case Wheel::State::WAITING:
      PORTE |= _BV(PE7);
    case Wheel::State::SPINNING:
      Poofers::spinBeat();
      // reset stop timer
      TCNT4 = 0;
    default:
      break;

      /*

      In timed-sequence, the code clocks though the byte array as discussed
      above, but when the array counter reaches 0, the code disables the beat
      counter interrupt, enables the sensor event interrupt, transitioning back
      to waiting-to-start.
      */
      // On tempo trigger
      //    iterate through poofer finale poses
  }
}
ISR(INT2_vect, ISR_ALIASOF(INT0_vect));

// Only happens when waiting
// On input capture interrupt aka
//     read ICR3 to see how much time has passed. Move to next or reset
ISR(TIMER3_CAPT_vect) {
  unsigned int timePassed = ICR3;
  if (Wheel::currentState == Wheel::State::WAITING &&
      timePassed < WHEEL_START_GAP) {
    Wheel::spinningSetup();
  }
  TCNT3 = 0;
}

// Stop timeout counter
ISR(TIMER4_COMPA_vect) {
  Wheel::finaleSetup();
  // disable wheel spin relay
  PORTC &= ~(_BV(PC0));
}

// Finale Tempo (or otherwise regular beat)
ISR(TIMER5_COMPA_vect) {
  bool end = Poofers::finaleBeat();
  if (end == true) {
    Wheel::finaleTeardown();
    Wheel::waitingSetup();
  }
}

/* Main */
//   Mode switch

ISR(PCINT0_vect) {
  Mode = PINB;  // update Mode

  if (lastMode != Mode) {
    // Teardown previous mode
    switch (lastMode) {
      case STANDBY:
        StandbyMode::teardown();
        break;
      case MANUAL:
        ManualMode::teardown();
        // PORTA &= ~(0b10111111);
        break;
      case SHOW:
      case AUDIO_SHOW:
        ShowMode::teardown();
        break;
      case BEAT:
      case AUDIO_BEAT:
        BeatMode::teardown();
        break;
      default:
        Mode = ModeList::STANDBY;
        break;
    }
    lastMode = Mode;
    PORTA = lastMode;

    // Setup
    switch (Mode) {
      case STANDBY:
        StandbyMode::setup();
        break;
      case MANUAL:
        ManualMode::setup();
        // PORTA &= ~(0b01111111);
        break;
      case SHOW:
      case AUDIO_SHOW:
        ShowMode::setup();
        break;
      case BEAT:
      case AUDIO_BEAT:
        BeatMode::setup();
        break;
      default:
        Mode = ModeList::STANDBY;
        break;
    }
  }
}

int main() {
  // enable global interrupt
  // SREG |= _BV(SREG_I);
  cli();

  // set up solenoid outputs
  DDRA = 0xFF;  // all pins to OUTPUT

  // === set up the mode select switch
  //  DDRB defaults to input
  DDRB = 0x00;
  // enable pin change interrupt 0
  PCICR |= _BV(PCIE0);
  PCMSK0 = 0b00000111;
  // set pull-ups
  PORTB = 0b00000111;
  // === /mode select

  // set up manual switches
  // DDRK defaults to input
  DDRK = 0x00;
  PCMSK2 = 0b01111111;  // select 7 pins for PCIE2
  PORTK = 0b01111111;   // 7 pull up resistors
  PORTA = 0b01111111;   // Set poofer defaults

  // set up auxiliary outputs
  DDRC =
      _BV(PC1) | _BV(PC0);  // wheel state output (0), button light output (1)

  // set up sensors
  // set beatbutton(2), hall sensor(1), hall sensor(0) to input
  // Only using PD0 Hall Sensor (DJT)
  DDRD = _BV(PD2) | _BV(PD0);
  PORTD = _BV(PD2) | _BV(PD0);  // set pull ups

  // initialize timers
  // all prescalers set to (freq/1024)
  // timer 3 - input for hallsensors/beatbutton
  TCCR3B |= _BV(CS32) | _BV(CS30);
  // timer 4 - output for stop-timeout
  TCCR4B |= _BV(CS42) | _BV(CS40);
  OCR4A = WHEEL_STOP_TIMEOUT;  // 1s
  // timer 5 - tempo for finale
  TCCR5B |= _BV(CS52) | _BV(CS50);
  OCR5A = FINALE_TEMPO;
  sei();

  // Mode loop
  for (;;) {
  }
}
