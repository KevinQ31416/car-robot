/*************************************************************
Motor Shield 2-Channel DC Motor Demo
by Randy Sarafan

For more information see:
https://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/

*************************************************************/
/*#include <AFMotor_R4.h>

// Define motor ports
AF_DCMotor motorLeft(1);  // Connected to M1
AF_DCMotor motorRight(2); // Connected to M2

// Define ultrasonic sensor pins
const int trigPin = A0;
const int echoPin = A1;

// Distance threshold for stopping (in centimeters)
const int stopDistance = 25; 

void setup() {

  Serial.begin(9600);
  Serial.println("--- Robot Initialized ---");

  // Initialize sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Set initial speed (0 to 255)
  motorLeft.setSpeed(200);
  motorRight.setSpeed(200);
}

void loop() {
  long duration;
  int distance;

  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond pulse to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin return time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  Serial.print("Measured Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Decision logic
  if (distance > 0 && distance < stopDistance) {
    // Obstacle detected: Stop immediately
    Serial.println("Status: Obstacle detected! Motors stopped.");
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
  } else {
    // Path is clear: Move forward
     Serial.println("Status: Path clear. Moving forward...");
    motorLeft.run(FORWARD);
    motorRight.run(FORWARD);
  }

  delay(1500); // Short delay for sensor stability
}*/
const int trigPin=A10;
const int echoPin=A11;

int DirA = 12;
int BrkA = 9;
int PwmA = 3;

int DirB = 13;
int BrkB = 8;
int PwmB = 11;

const int safetyDistance=20;

long stopmsg;
long forwardmsg;

long count;

void setup() {
    Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Setup Channel A
  pinMode(DirA, OUTPUT); //DIR A pin
  pinMode(BrkA, OUTPUT); //ON/OFF A pin
  pinMode(PwmA, OUTPUT); //Pwm A pin
 

  //Setup Channel B
  pinMode(DirB, OUTPUT); //DIR B pin
  pinMode(BrkB, OUTPUT); //ON/OFF B pin
  pinMode(PwmB, OUTPUT); //Pwm B pin
}
void loop() {

  long totalDistance = 0;
  int validReadings = 0;
  for(int i = 0; i < 3; i++){
    long reading = getDistance();
    if(reading>0){
      //ignoring timeout errors so they dont corrupt the average
      totalDistance += reading;
      validReadings++;
    }
    delay(20);
  }
  long averageDistance = (validReadings > 0) ? (totalDistance / validReadings) : 0;

  Serial.print("Average Distance: ");
  Serial.print(averageDistance);
  Serial.println(" cm");

 if (averageDistance >= 0 && averageDistance <= safetyDistance ) 
 {
    moveStop();
    if(stopmsg<1)
    {
      stopmsg=1;
      forwardmsg=0;
    }
    
  } 
  else 
  {
    moveForward();
    count++;
    if(forwardmsg<1)
    {
      forwardmsg=1;
      stopmsg=0;
    }
    
  }
  if(stopmsg==1){
    Serial.println("Obstacle Detected! Stopping Robot.");
    stopmsg=2;
  }
  if(forwardmsg==1){
    Serial.println("It is moving ahead");
    forwardmsg=2;
  }
delay(100);
}

long getDistance() {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout

long cm = (duration * 0.034) / 2;
  
  return cm;

}

// moved from the lopp here so that we only execute once
void moveForward() 
{
  //Motor A forward @ full speed
  digitalWrite(DirA, HIGH); //Establishes forward direction of Channel A
  digitalWrite(BrkA, LOW);   //Disengage the Brake for Channel A
  analogWrite(PwmA, 215);   //Spins the motor on Channel A at full speed

  //Motor B forward @ half speed
  digitalWrite(DirB, HIGH);  //Establishes forward direction of Channel B
  digitalWrite(BrkB, LOW);   //Disengage the Brake for Channel B
  analogWrite(PwmB, 255);    //Spins the motor on Channel B at half speed
  
}

  void moveStop()
  {

  /*digitalWrite(BrkA, HIGH);  //Engage the Brake for Channel A
  digitalWrite(BrkB, HIGH);  //Engage the Brake for Channel B*/

    //Motor A forward @ full speed
  digitalWrite(DirA, LOW); //Establishes forward direction of Channel A
  digitalWrite(BrkA, HIGH);   //Engage the Brake for Channel A
  analogWrite(PwmA, 0);   //Spins the motor on Channel A at full speed

  //Motor B forward @ half speed
  digitalWrite(DirB, LOW);  //Establishes forward direction of Channel B
  digitalWrite(BrkB, HIGH);   //Engage the Brake for Channel B
  analogWrite(PwmB, 0);    //Spins the motor on Channel B at half speed
  
  } 



  // Kept empty so the code does not repeat

/*
int DirA = 12;
int BrkA = 9;
int PwmA = 3;


int DirB = 13;
int BrkB = 8;
int PwmB = 11;


void setup() {
  
  //Setup Channel A
  pinMode(DirA, OUTPUT); //DIR A pin
  pinMode(BrkA, OUTPUT); //ON/OFF A pin
  pinMode(PwmA, OUTPUT); //Pwm A pin
 

  //Setup Channel B
  pinMode(DirB, OUTPUT); //DIR B pin
  pinMode(BrkB, OUTPUT); //ON/OFF B pin
  pinMode(PwmB, OUTPUT); //Pwm B pin

// moved from the lopp here so that we only execute once

  //Motor A forward @ full speed
  digitalWrite(DirA, HIGH); //Establishes forward direction of Channel A
  digitalWrite(BrkA, LOW);   //Disengage the Brake for Channel A
  analogWrite(PwmA, 215);   //Spins the motor on Channel A at full speed

  //Motor B forward @ half speed
  digitalWrite(DirB, HIGH);  //Establishes forward direction of Channel B
  digitalWrite(BrkB, LOW);   //Disengage the Brake for Channel B
  analogWrite(PwmB, 255);    //Spins the motor on Channel B at half speed

  
  delay(3000);

  
  digitalWrite(BrkA, HIGH);  //Engage the Brake for Channel A
  digitalWrite(BrkB, HIGH);  //Engage the Brake for Channel B


  delay(1000);
  
  
  //Motor A backward @ full speed
  digitalWrite(DirA, LOW);  //Establishes backward direction of Channel A
  digitalWrite(BrkA, LOW);   //Disengage the Brake for Channel A
  analogWrite(PwmA, 215);    //Spins the motor on Channel A at half speed
  
  //Motor B backward @ full speed
  digitalWrite(DirB, LOW); //Establishes backward direction of Channel B
  digitalWrite(BrkB, LOW);   //Disengage the Brake for Channel B
  analogWrite(PwmB, 255);   //Spins the motor on Channel B at full speed
  
  
  delay(3000);
  
  
  digitalWrite(BrkA, HIGH);  //Engage the Brake for Channel A
  digitalWrite(BrkB, HIGH);  //Engage the Brake for Channel B
  
  
  delay(1000);


  
}

void loop(){*/

  /*

  //Motor A forward @ full speed
  digitalWrite(DirA, HIGH); //Establishes forward direction of Channel A
  digitalWrite(BrkA, LOW);   //Disengage the Brake for Channel A
  analogWrite(PwmA, 255);   //Spins the motor on Channel A at full speed

  //Motor B backward @ half speed
  digitalWrite(DirB, HIGH);  //Establishes backward direction of Channel B
  digitalWrite(BrkB, LOW);   //Disengage the Brake for Channel B
  analogWrite(PwmB, 255);    //Spins the motor on Channel B at half speed

  
  delay(3000);

  
  digitalWrite(BrkA, HIGH);  //Engage the Brake for Channel A
  digitalWrite(BrkB, HIGH);  //Engage the Brake for Channel B


  delay(1000);
  
  
  //Motor A forward @ full speed
  digitalWrite(DirA, LOW);  //Establishes backward direction of Channel A
  digitalWrite(BrkA, LOW);   //Disengage the Brake for Channel A
  analogWrite(PwmA, 255);    //Spins the motor on Channel A at half speed
  
  //Motor B forward @ full speed
  digitalWrite(DirB, LOW); //Establishes forward direction of Channel B
  digitalWrite(BrkB, LOW);   //Disengage the Brake for Channel B
  analogWrite(PwmB, 255);   //Spins the motor on Channel B at full speed
  
  
  delay(3000);
  
  
  digitalWrite(BrkA, HIGH);  //Engage the Brake for Channel A
  digitalWrite(BrkB, HIGH);  //Engage the Brake for Channel B
  
  
  delay(1000);
  
  */
/*}*/