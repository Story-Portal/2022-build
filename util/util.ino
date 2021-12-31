// AVR Compatible boards
#include <Arduino_AVRSTL.h>
// Non-AVR boards
//#include <ArduinoSTL.h>
#include <map>
#include <functional>
#include <Bounce2.h>

void testPrint(int inp) {
  Serial.println(inp);
}

void testPrint2(int inp) {
  Serial.println(inp);
}

void testPrint3(int inp) {
  Serial.println(inp);
}

void testPrint4(int inp) {
  Serial.println(inp);
}

namespace Event{
  // Mapping from event name = pin number
  enum Type {
    test = 0,
    test1, 
    test2,
    test3
  };

  static const Type List[] = {test, test1, test2, test3};

  static const std::map<int, void (*)(int)> FunctionMap = {
    {0, &testPrint},
    {1, &testPrint2},
    {2, &testPrint3},
    {3, &testPrint4},
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
