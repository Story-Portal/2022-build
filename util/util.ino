// AVR Compatible boards
#include <Arduino_AVRSTL.h>
// Non-AVR boards
#include <ArduinoSTL.h>
#include <map>
#include <functional>
#include <Bounce2.h>


void testPrint() {
  Serial.print(42);
}

//std::map<int, std::function<void()>> functionMap = {
std::map<int, void (*)()> functionMap = {
  {0, &testPrint},
};

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  bool hasRun = false;
  if (!hasRun){
    auto fn = functionMap.find(0);
    if ( fn != functionMap.end()) {
      fn->second();
    }
    hasRun = true;
  }
  
}
