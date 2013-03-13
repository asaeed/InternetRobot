
#include "Arduino.h"
#include "MotorDriver.h"

MotorDriver::MotorDriver(int pinFwd,int pinRev, int pinPwm)
{
  pinMode(pinFwd, OUTPUT);
  pinMode(pinRev, OUTPUT);
  pinMode(pinPwm, OUTPUT);
  _pinFwd = pinFwd;
  _pinRev = pinRev;
  _pinPwm = pinPwm;
}

void MotorDriver::setSpeed(int speed)
{
  _speed = constrain(speed, -100, 100);
  _pwm = map(abs(_speed), 0, 100, 50, 255);

  if (_speed > 0)
  {
    digitalWrite(_pinFwd, HIGH );
    digitalWrite(_pinRev, LOW);
  }
  else if (_speed == 0)
  {
    digitalWrite(_pinFwd, HIGH);
    digitalWrite(_pinRev, HIGH);
  }
  else if (_speed < 0)
  {
    digitalWrite(_pinFwd, LOW);
    digitalWrite(_pinRev, HIGH);
  }
  
  analogWrite (_pinPwm, _pwm);
}

int MotorDriver::getSpeed()
{
  return _speed;
}
