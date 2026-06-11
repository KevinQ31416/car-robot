const int trigPin = A10; 
const int echoPin = A11; 

// Arduino Motor Shield R3 Pins
int DirA = 12; 
int BrkA = 9; 
int PwmA = 3; 
int DirB = 13; 
int BrkB = 8; 
int PwmB = 11; 

int dist_disp_dly;
int avg_dist_loop_count;
int obstacle;
int CmdMem;

const int safetyDistance = 50; // Emergency stop distance in cm
char command = '5';   // Default state: Stopped
char FwdCmd = '1';   // Default state: Stopped

long totalDistance;
int validReadings;
long averageDistance;         

void setup() {
  Serial.begin(9600);   // USB Debugging
  Serial1.begin(9600);  // HC-05 Bluetooth Module
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(DirA, OUTPUT);
  pinMode(BrkA, OUTPUT);
  pinMode(PwmA, OUTPUT);
  
  pinMode(DirB, OUTPUT);
  pinMode(BrkB, OUTPUT);
  pinMode(PwmB, OUTPUT);
  
  moveStop(); 
}

void loop() {
  // 1. Sensor Reading & Filtering
  
 
  long reading = getDistance();
  if(reading > 0 && reading <= 999){ // Filter out timeout (0) and false high readings
    totalDistance += reading;
    validReadings++;
  }
  

  avg_dist_loop_count++;
  if(avg_dist_loop_count>3)
  {
    avg_dist_loop_count=0;
    //averageDistance = (validReadings > 0) ? (totalDistance / validReadings) : 999;
    averageDistance=totalDistance/validReadings;
    validReadings=0;
    totalDistance=0;
  }

  dist_disp_dly++;
  if(dist_disp_dly>10)
  {
    dist_disp_dly=0;
    Serial.print(averageDistance);
    Serial.print(" ");
    Serial.println(CmdMem);
  }

    // 3. Safety Override & Movement Logic
  // If an obstacle is detected closer than safety limit AND user tries to move forward
  if (averageDistance <= safetyDistance) 
  {
    if(CmdMem==1)
    {
      command='5';
      moveStop();
      CmdMem=5;
    }
    obstacle=1;
    Serial.print("Warning: Obstacle Detected! Forward blocked. Command is now :");
    Serial.println(command);
    Serial.print("Avg Distance :");
    Serial.println(averageDistance);
  }
  else
  {
    obstacle=0;
  } 

  // 2. Read Bluetooth Command
  if (Serial1.available() > 0) {
    command = Serial1.read(); 
    Serial.print("Received Command: ");
    Serial.println(command);
  }

  // Execute Bluetooth command if safe
  switch (command) {
    case '1': // UP
    if(obstacle==0)
    {
      moveForward();
      CmdMem=1;
    }
      Serial.println(averageDistance);
      break;
    case '2': // DOWN
      moveBackward();
      CmdMem=2;
      break;
    case '3': // LEFT
      turnLeft();
      CmdMem=3;
      break;
    case '4': // RIGHT
      turnRight();
      CmdMem=4;
      break;
    case '5': // Center / Stop
      moveStop();
      CmdMem=5;
      break;
    default:
      // Do nothing for unknown characters
      break;
  }
  
  
  delay(300); // Balanced loop delay for smooth response
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 20ms timeout (~340cm max range) keeps the loop snappy
  long duration = pulseIn(echoPin, HIGH, 20000); 
  if (duration == 0) return 999; // Return high distance if no echo
  
  return (duration * 0.034) / 2;
}

void moveForward() {
  digitalWrite(DirA, HIGH);
  digitalWrite(BrkA, LOW);
  analogWrite(PwmA, 195); 
  digitalWrite(DirB, HIGH);
  digitalWrite(BrkB, LOW);
  analogWrite(PwmB, 235);
}

void moveBackward() {
  digitalWrite(DirA, LOW);
  digitalWrite(BrkA, LOW);
  analogWrite(PwmA, 190); 
  digitalWrite(DirB, LOW);
  digitalWrite(BrkB, LOW);
  analogWrite(PwmB, 230);
}

void turnRight() {
  // Motor A forward, Motor B brake (Pivot right)
  digitalWrite(DirA, HIGH);
  digitalWrite(BrkA, LOW);
  analogWrite(PwmA, 215);
  
  digitalWrite(DirB, HIGH);
  digitalWrite(BrkB, HIGH);
  analogWrite(PwmB, 0);
}

void turnLeft() {
  // Motor A brake, Motor B forward (Pivot left)
  digitalWrite(DirA, HIGH);
  digitalWrite(BrkA, HIGH);
  analogWrite(PwmA, 0);
  
  digitalWrite(DirB, HIGH);
  digitalWrite(BrkB, LOW);
  analogWrite(PwmB, 255);
}

void moveStop() {
  digitalWrite(BrkA, HIGH); 
  digitalWrite(BrkB, HIGH); 
  analogWrite(PwmA, 0);
  analogWrite(PwmB, 0);
}
/*const int trigPin = A10; 
const int echoPin = A11; 

// Arduino Motor Shield R3 Pins
int DirA = 12; 
int BrkA = 9; 
int PwmA = 3; 
int DirB = 13; 
int BrkB = 8; 
int PwmB = 11; 

const int safetyDistance = 20; // Stopping threshold in cm
long stopmsg = 0; 
long forwardmsg = 0; 
long count = 0; 

void setup() { 
  Serial.begin(9600); 
  
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  
  // Setup Channel A
  pinMode(DirA, OUTPUT); 
  pinMode(BrkA, OUTPUT); 
  pinMode(PwmA, OUTPUT); 
  
  // Setup Channel B
  pinMode(DirB, OUTPUT); 
  pinMode(BrkB, OUTPUT); 
  pinMode(PwmB, OUTPUT); 
} 

void loop() { 
  long totalDistance = 0; 
  int validReadings = 0; 
  
  for(int i = 0; i < 3; i++){ 
    long reading = getDistance(); 
    if(reading > 0 && reading < 999){ // Ignores timeout errors
      totalDistance += reading; 
      validReadings++; 
    } 
    delay(20); 
  } 
  
  // If all readings failed, assume path is clear (999 cm)
  long averageDistance = (validReadings > 0) ? (totalDistance / validReadings) : 999; 
  
  Serial.print("Average Distance: "); 
  Serial.print(averageDistance); 
  Serial.println(" cm"); 
  
  // Obstacle 
  if (averageDistance > 0 && averageDistance <= safetyDistance ) { 
    if(stopmsg < 1) { 
      stopmsg = 1; 
      forwardmsg = 0; 
    }
    
    Serial.println("Obstacle Detected! going back n turnin.");
    
    moveStop(); 
    delay(600);
    
    moveBackward();
    delay(1400); // Backing
    
    moveStop();
    delay(600);
    
    turnRight();
    delay(900);
    turnrighty(); // turn time
    delay(3000);
    moveStop();
    delay(1000);
    
  } else { 
    moveForward(); 
    count++; 
    if(forwardmsg < 1) { 
      forwardmsg = 1; 
      stopmsg = 0; 
    } 
  } 
  
  if(forwardmsg == 1){ 
    Serial.println("Path clear. Moving ahead..."); 
    forwardmsg = 2; 
  } 
  
  delay(50); 
} 

long getDistance() { 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout 
  
  if (duration == 0) {
    return 999; // Safe value if out of sensor range
  }
  
  long cm = (duration * 0.034) / 2; 
  return cm; 
} 

void moveForward() { 
  digitalWrite(DirA, HIGH); 
  digitalWrite(BrkA, LOW); 
  analogWrite(PwmA, 215); 
  
  digitalWrite(DirB, HIGH); 
  digitalWrite(BrkB, LOW); 
  analogWrite(PwmB, 255); 
} 

void moveBackward() { 
  digitalWrite(DirA, LOW); // Reverse Direction
  digitalWrite(BrkA, LOW); 
  analogWrite(PwmA, 200);  // Slightly slower speed for stability
  
  digitalWrite(DirB, LOW); // Reverse Direction
  digitalWrite(BrkB, LOW); 
  analogWrite(PwmB, 230); 
}

void turnRight() {
  // Motor A drives Forward, Motor B drives Backward to spin on the spot
  digitalWrite(DirA, HIGH); 
  digitalWrite(BrkA, LOW); 
  analogWrite(PwmA, 215); 
  
  digitalWrite(DirB, HIGH); 
  digitalWrite(BrkB, LOW); 
  analogWrite(PwmB, 255); 
}
void turnrighty(){
  digitalWrite(DirA, HIGH);
  digitalWrite(BrkA, LOW);
  analogWrite(PwmA, 225);

  digitalWrite(DirA, HIGH);
  digitalWrite(BrkA, HIGH);
  analogWrite(PwmA, 0);
}

void moveStop() { 
  digitalWrite(BrkA, HIGH); // Force hard brake
  digitalWrite(BrkB, HIGH); 
  analogWrite(PwmA, 0); 
  analogWrite(PwmB, 0); 
}*/
/*
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
  //digitalWrite(DirA, LOW); //Establishes forward direction of Channel A
  //digitalWrite(BrkA, HIGH);   //Engage the Brake for Channel A
  //analogWrite(PwmA, 0);   //Spins the motor on Channel A at full speed

  //Motor B forward @ half speed
  //digitalWrite(DirB, LOW);  //Establishes forward direction of Channel B
  //digitalWrite(BrkB, HIGH);   //Engage the Brake for Channel B
  //analogWrite(PwmB, 0);    //Spins the motor on Channel B at half speed
  
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
