#include <Bounce2.h>
// Mapping pins
enum Pin {
  test = 0,
  TOGGLE_LED = 2,
  LED = 13,
};

namespace LedToggleButton {
int pin;
int ledState;
int lastButtonState;
void init(int inPin, int outPin) {
  pinMode(inPin, INPUT);
  pinMode(outPin, OUTPUT);
  pin = outPin;
  ledState = false;
}
void toggle() {
  ledState = !ledState;
  digitalWrite(pin, ledState);
}
void update(int val) {
  if (val != lastButtonState && val == HIGH) {
    toggle();
  }
  lastButtonState = val;
  delay(5);
}
}  // namespace LedToggleButton

void testPrint(int inp) {
  Serial.println(inp);
}

namespace Event {
struct Obj {
  Pin pin;
  void (*function)(int inp);
};

static Obj List[] = {
    {Pin::test, testPrint},
    {Pin::TOGGLE_LED, (LedToggleButton::update)},
};
}  // namespace Event

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LedToggleButton::init(Pin::TOGGLE_LED, Pin::LED);
}

void loop() {
  // put your main code here, to run repeatedly:

  // For each possible event, check if there is a function for that event
  // if there is a function, read the pin for that event
  // pass the value to the relevant function
  for (const Event::Obj event : Event::List) {
    int val = digitalRead(event.pin);
    event.function(val);
  }
}
