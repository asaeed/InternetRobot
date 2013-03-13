
#include <aJSON.h>
#include <HMC5883L.h>
#include <MotorDriver.h>
#include <Wire.h>
#include <Servo.h>

#define LED_PIN 13

#define motorAPWM 11
#define motorAIN1 12
#define motorAIN2 13
#define motorBPWM 10
#define motorBIN1 9
#define motorBIN2 8

#define neckMotorPin 7 // pwm only?

MotorDriver leftMotor = MotorDriver(motorAIN1, motorAIN2, motorAPWM);
MotorDriver rightMotor = MotorDriver(motorBIN1, motorBIN2, motorBPWM);
HMC5883L compass = HMC5883L();
Servo neckMotor;
float neckAngle;

int msgByte= -1;         // incoming byte
const int msgSize = 50;  // max message size
char msgArray[msgSize];  // array for incoming data
int msgPos = 0;          // current position

void setup() {
    Serial.begin(9600);
    Serial2.begin(9600);
    Wire.begin();
    pinMode(LED_PIN, OUTPUT);
    
    setupHMC5883L();
    neckMotor.attach(neckMotorPin);
    neckAngle = 90;
    neckMotor.write(neckAngle-1);
    
    Serial.println("setup done");
    //freeMem("starting");
}

void loop() {
  handleSerial();
}

void receiveData() {
  //freeMem("receive start");
  Serial.println(msgArray);
  aJsonObject* root = aJson.parse(msgArray);
  aJsonObject* neck = aJson.getObjectItem(root, "neck");
  neckAngle = neck->valuefloat;
  //Serial.println(neckAngle);
  neckMotor.write(neckAngle);
  
  /*
  int power = 60;
  float x, y;
  if (dataIn[5] == '2') {
    if (x > .25 && x < .75 && y < .25) { // fwd
      leftMotor.setSpeed(power);
      rightMotor.setSpeed(power);
      delay(100);
      leftMotor.setSpeed(0);
      rightMotor.setSpeed(0);
    }
    if (x > .25 && x < .75 && y > .75) { // back
      leftMotor.setSpeed(-power);
      rightMotor.setSpeed(-power);
      delay(100);
      leftMotor.setSpeed(0);
      rightMotor.setSpeed(0);
    }
    if (x < .25 && y > .25 && y < .75) { // turn left
      leftMotor.setSpeed(-power);
      rightMotor.setSpeed(power);
      delay(100);
      leftMotor.setSpeed(0);
      rightMotor.setSpeed(0);
    }
    if (x > .75 && y > .25 && y < .75) { // turn right
      leftMotor.setSpeed(power);
      rightMotor.setSpeed(-power);
      delay(100);
      leftMotor.setSpeed(0);
      rightMotor.setSpeed(0);
    }
  }
  */
  
  aJson.deleteItem(root);
  //freeMem("receive end");
}

void sendData() {
  float heading = getHeading();
  
  //freeMem("send start");
  aJsonObject *root = aJson.createObject();
  aJsonObject *neck;
  aJson.addItemToObject(root, "heading",  neck = aJson.createItem(heading)); 
  
  char* json = aJson.print(root);
  Serial2.println(json);
  free(json);
  
  aJson.deleteItem(root);
  //freeMem("send end");
}

void handleSerial() {  
  if (Serial2.available() > 0) {
    digitalWrite(LED_PIN, HIGH);
    msgByte = Serial2.read();
    
    if (msgByte != '\n') {
      // add incoming byte to array
      msgArray[msgPos] = msgByte;
      msgPos++;
    } else {
      // reached end of line
      msgArray[msgPos] = 0;
      
      // here the message is processed
      receiveData();
      sendData();
    
      // reset byte array
      for (int c = 0; c < msgSize; c++) 
        msgArray[c] = ' ';

      msgPos = 0;
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void setupHMC5883L(){
  int error; 
  error = compass.SetScale(1.3);
  if(error != 0) Serial.println(compass.GetErrorText(error));
  error = compass.SetMeasurementMode(Measurement_Continuous);
  if(error != 0) Serial.println(compass.GetErrorText(error));
}

float getHeading(){
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  if(heading < 0) heading += 2*PI;
  if(heading > 2*PI) heading -= 2*PI;
  return heading * RAD_TO_DEG;
}


//Code to print out the free memory

extern unsigned int __heap_start;
extern void *__brkval;

/*
 * The free list structure as maintained by the 
 * avr-libc memory allocation routines.
 */
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

/* The head of the free list structure */
extern struct __freelist *__flp;

/* Calculates the size of the free list */
int freeListSize() {
  struct __freelist* current;
  int total = 0;

  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  return total;
}

int freeMemory() {
  int free_memory;

  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}

void freeMem(char* message) {
  Serial.print(message);
  Serial.print(":\t");
  Serial.println(freeMemory());
}
