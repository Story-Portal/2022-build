// AVR Compatible boards
#include <Arduino_AVRSTL.h>
// Non-AVR boards
//#include <ArduinoSTL.h>
#include <map>
#include <Bounce2.h>

void testPrint(int inp) {
  Serial.println(inp);
}

typedef void (*IntFunctionWithOneParameter) (int a);

namespace Event{
  // Mapping from event name = pin number
  enum Type {
    test = 0,
  };

  static const Type List[] = {test};

  static const std::map<int, IntFunctionWithOneParameter> FunctionMap = {
    {0, &testPrint},
  };
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // For each possible event, check if there is a function for that event
  // if there is a function, read the pin for that event
  // pass the value to the relevant function
  for ( const auto e: Event::List){
    auto fn = Event::FunctionMap.find(e);
    if ( fn != Event::FunctionMap.end()) {
      int val = digitalRead(e);
      fn->second(val);
    }
  }
  
}