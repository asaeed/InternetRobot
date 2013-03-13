
#ifndef MotorDriver_h
#define MotorDriver_h

#include "Arduino.h"

class MotorDriver
{
public:
  MotorDriver(int pinFwd,int pinRev,int pinPwm);
  void setSpeed(int speed);
  int getSpeed();

private:
  int _pinFwd;
  int _pinRev;
  int _pinPwm;
  int _speed;
  int _pwm;
};
#endif 
