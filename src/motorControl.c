#include <Servo.h>
#include <LiquidCrystal.h>

Servo servoX;
Servo servoY;
LIDARLite lidar;
// Minimum and maximum servo angle in degrees
// Modify to avoid hitting limits imposed by pan/tilt bracket geometry
int minPosX = 0;
int maxPosX = 180;
int minPosY = 26;
int maxPosY = 27;

int buttonPin = A0;
int buttonValue = 0;
int buttonThreshold = 50;
int lastPosX = 0;
int lastPosY = 0;
int loopCount = 0;
int radius = 0;
int lastRadius = 0;
boolean selectButtonPressed = false;
boolean scanning = false;
boolean scanDirection = false;
int scanIncrement = 1;
int posX = (maxPosX + minPosX) / 2;
int posY = (maxPosY + minPosY) / 4;
float pi = 3.14159265;
float deg2rad = pi / 180.0;

void setup() {
  lidar.begin(0, true);
  lidar.configure(0);
  servoX.attach(2);
  servoY.attach(3);
  servoX.write(posX);
  servoY.write(posY);
  Serial.begin(9600);
}

void loop() {

  buttonValue = analogRead(buttonPin);

  if (scanning) {
    if (abs(buttonValue - 741) < buttonThreshold) {
      if (!selectButtonPressed) {
        // switch to manual scan mode
        selectButtonPressed = true;
        scanning = false;
      }
    } else {
      selectButtonPressed = false;
    }
    if (scanDirection) {
      posX += scanIncrement;
    } else {
      posX -= scanIncrement;
    }
    if (posX > maxPosX || posX < minPosX) {
      // hit limit X limit, reverse auto scan direction
      scanDirection = !scanDirection;
      posY += scanIncrement;
      if (posY > maxPosY) {
        // completed auto scan, return to manual scan mode
        scanning = false;
      }
    }
  } else {
    if (abs(buttonValue - 741) < buttonThreshold) {
      if (!selectButtonPressed) {
        // switch to auto scan mode
        selectButtonPressed = true;
        scanning = true;
        posX = minPosX;
        posY = minPosY;
        scanDirection = true;
      }
    } else if (abs(buttonValue - 505) < buttonThreshold) {
      // manual scan left
      posX += 1;
    } else if (abs(buttonValue - 329) < buttonThreshold) {
      // manual scan down
      posY -= 1;
    } else if (abs(buttonValue - 145) < buttonThreshold) {
      // manual scan up
      posY += 1;
    } else if (abs(buttonValue - 0) < buttonThreshold) {
      // manual scan right
      posX -= 1;
    } else {
      selectButtonPressed = false;
    }
  }

  posX = min(max(posX, minPosX), maxPosX);
  posY = min(max(posY, minPosY), maxPosY);
  bool moved = moveServos();
  displayPosition();

  loopCount += 1;
  if (loopCount % 100 == 0) {
    // recalibrate scanner every 100 scans
    radius = lidar.distance();
  } else {
    radius = lidar.distance(false);
  }
  if (abs(radius - lastRadius) > 2)
  {
    lastRadius = radius;
  }
  if (scanning || moved) {
    float azimuth = posX * deg2rad;
    float elevation = (180 - maxPosY + posY) * deg2rad;
    double x = radius * sin(elevation) * cos(azimuth);
    double y = radius * sin(elevation) * sin(azimuth);
    double z = radius * cos(elevation);
    Serial.print(String(-x, 5) + " " + String(y, 5) + " " + String(-z, 5));
  }
}

bool moveServos()
{
  bool moved = false;
  static int lastPosX;
  static int lastPosY;
  int delta = 0;  
  if (posX != lastPosX) {
    delta += abs(posX - lastPosX);
    servoX.write(posX);
    lastPosX = posX;
    moved = true;
  }
  if (posY != lastPosY) {
    delta += abs(posY - lastPosY);
    servoY.write(posY);
    lastPosY = posY;
    moved = true;
  }
  delay(30);
  return moved;
}

void displayPosition()
{
  static int lastPosX;
  static int lastPosY;
  if (posX != lastPosX) {
    lastPosX = posX;
  }
  if (posY != lastPosY) {
    lastPosY = posY;
  }  
}