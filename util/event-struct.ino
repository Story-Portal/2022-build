#include <Bounce2.h>

void testPrint(int inp) {
  Serial.println(inp);
}

namespace Event {
// Mapping from event name = pin number
enum Type {
  test = 0,
};

struct Obj {
  Type pin;
  void (*function)(int inp);
};

static const Obj List[] = {
    {test, testPrint},
};
}  // namespace Event

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
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
