// C++ code
//
/*
  Sweep

  by BARRAGAN <http://barraganstudio.com>
  This example code is in the public domain.

  modified 8 Nov 2013  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>


class AbstractServo {
  
  int pin;
  /*
  	the pulse width, in microseconds, corresponding to 
    the minimum (0-degree) angle on the servo 
    (defaults to 544)
  */
  int minPulseWidth; // defaults to 544
  /* 
    the pulse width, in microseconds, corresponding to 
    the maximum (180-degree) angle on the servo 
    (defaults to 2400)
  */
  int maxPulseWidth;
  
  protected:
  Servo servo;

  bool directionClockwise;

  public:
  AbstractServo(int pin, bool direction):
    pin(pin),
    directionClockwise(direction),
    minPulseWidth {544},
    maxPulseWidth {2400} {}
  AbstractServo(int pin, int min, int max, bool direction):
    pin(pin),
    directionClockwise(direction),
    minPulseWidth(min),
    maxPulseWidth(max) {}
  
  virtual void setup() = 0;
  
  virtual void loop() = 0;
};

void AbstractServo::setup(){
  Serial.print("Attaching Servo to pin: ");
  Serial.println(this->pin);
  this->servo.attach(this->pin, this->minPulseWidth, this->maxPulseWidth);
};

class StandardServo: public AbstractServo {
  // time between position updates in ms
  int delay;
  int lastTime;
  int currentPosition;
  
  // if false, reaching end resets 
  bool bounce;
  
  int upperBound = 180;
  int lowerBound = 0;
  
  void incrementPosition(int amount){
  	if(this->directionClockwise){
      this->currentPosition += amount;
    } else {
      this->currentPosition -= amount;
    }
  }

  int updatePosition(int amount = 1){
    this->incrementPosition(amount);
    
    
    // If bounds are surpassed
    if(this->currentPosition > this->upperBound) {
      if(this->bounce) {
        int overflow = this->currentPosition - this->upperBound;
        this->directionClockwise = !this->directionClockwise;
        this->incrementPosition(overflow * 2);
      } else {
        this->currentPosition = this->lowerBound;
      }
    } else if (this->currentPosition < this->lowerBound) {
      if(this->bounce){
        int overflow = this->lowerBound - this->currentPosition;
        this->directionClockwise = !this->directionClockwise;
        this->incrementPosition(overflow * 2);
      } else {
	  	this->currentPosition = this->upperBound;
      }
    }
    // Serial.println(this->currentPosition);
    return this->currentPosition;
  }
  
  public:
  StandardServo(int pin, bool direction, int delay, bool bounce = true):
  	AbstractServo(pin, direction),
    delay(delay), bounce(bounce) {}
  
  StandardServo(int pin, int min, int max, bool direction, int delay, bool bounce = true):
    AbstractServo(pin, min, max, direction),
    delay(delay), bounce(bounce) {}
  
  void setup() {
    AbstractServo::setup();
    Serial.print("Setting up Standard Servo");
  	this->currentPosition = 0;
    this->servo.write(this->currentPosition);
    this->lastTime = millis();
  }

  void loop() {
    int currentTime = millis();
    int timePassed = currentTime - this->lastTime;
    if (timePassed >= delay) {
      int ticks = timePassed / delay;
      int tickRem = timePassed % delay;
	  this->lastTime = currentTime - tickRem;
      this->servo.write(this->updatePosition(ticks));
    }
  }
};

class ContinuousServo: public AbstractServo {
  
  // 0-100 %
  int speed;

  public:
  ContinuousServo(int pin, bool direction, int speed):
    AbstractServo(pin, direction),
    speed(speed) {}

  ContinuousServo(int pin, int min, int max, bool direction, int speed):
    AbstractServo(pin, min, max, direction),
    speed(speed) {}

  void setup(){
    AbstractServo::setup();
    this->servo.write(90);
  }
  
  // 0 full speed one way, 180 full speed the other
  void loop() {
    int speedProportion = 90 * this->speed / 100;
    if (this->directionClockwise){
      this->servo.write(90 + speedProportion);
    } else {
      this->servo.write(90 - speedProportion);
    }
  }
};

AbstractServo* servos[] = {
	new StandardServo(9, 500, 2500, true, 15),
};

void setup() {
  Serial.begin(9600);
  Serial.print("test\n");
  
  for (AbstractServo* servo : servos) {
    servo->setup();
  }

}

void loop() {
  for (AbstractServo* servo : servos) {
    servo->loop();
  }
}
