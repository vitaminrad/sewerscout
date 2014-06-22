#include <Wire.h>
#include <Servo.h>

Servo Xservo;
Servo Yservo;

int valX;
int prevValX;
int valY;
int prevValY;

int levelX;
int levelY;

int XservoAngle = 90;
int YservoAngle = 90;

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23

int Addr = 105;                 // I2C address of gyro
int x, y, z;

void setup(){
  Wire.begin();
  Serial.begin(9600);

  Xservo.attach(9);             // X axis Servo
//  Yservo.attach(10);            // Y axis Servo

  Xservo.write(XservoAngle);

  writeI2C(CTRL_REG1, 0x1F);    // Turn on all axes, disable power down
  writeI2C(CTRL_REG3, 0x08);    // Enable control ready signal
  writeI2C(CTRL_REG4, 0x80);    // Set scale (500 deg/sec)

  delay(600);                   // Wait to synchronize 
}

void loop(){
  getGyroValues();              // Get new values

  // Reduce noise
  valX = x / 114;               // Reduce Noise
  valY = y / 114;

  if (valX != prevValX) {
    levelX += valX;             // Accumulative Angle
    prevValX = valX;
  }

  if (valY != prevValY) {
    levelY += valY;            // Accumulative Angle
    prevValY = valY;
  }

  // Map gyro to servo...
  // Servos operate from angles 0 - 180
  XservoAngle = map(levelX,-7000,7000,0,179);
  YservoAngle = map(levelY,-7000,7000,0,179);
  
  Xservo.write(XservoAngle);
  Yservo.write(YservoAngle);

  delay(20);  // Short delay between reads
}

void getGyroValues () {
  byte MSB, LSB;

  MSB = readI2C(0x29);
  LSB = readI2C(0x28);
  x = ((MSB << 8) | LSB);

  MSB = readI2C(0x2B);
  LSB = readI2C(0x2A);
  y = ((MSB << 8) | LSB);

  MSB = readI2C(0x2D);
  LSB = readI2C(0x2C);
  z = ((MSB << 8) | LSB);
}

int readI2C (byte regAddr) {
    Wire.beginTransmission(Addr);
    Wire.write(regAddr);                // Register address to read
    Wire.endTransmission();             // Terminate request
    Wire.requestFrom(Addr, 1);          // Read a byte
    while(!Wire.available()) { };       // Wait for receipt
    return(Wire.read());                // Get result
}

void writeI2C (byte regAddr, byte val) {
    Wire.beginTransmission(Addr);
    Wire.write(regAddr);
    Wire.write(val);
    Wire.endTransmission();
}
