#include <Servo.h>

// Constants won't change
const int TRIG_PIN1 = 6;  // Sensor 1 TRIG pin
const int ECHO_PIN1 = 7;  // Sensor 1 ECHO pin
const int TRIG_PIN2 = 8;  // Sensor 2 TRIG pin
const int ECHO_PIN2 = 9;  // Sensor 2 ECHO pin
const int SERVO_PIN = 10; // Servo 
const int DISTANCE_THRESHOLD = 50;  
const int FULL_THRESHOLD = 30;     
const int RED_LED_PIN = 13; // Red LED 

Servo servo; // Servo

float duration_us1, distance_cm1;
float duration_us2, distance_cm2;

int comparator = 0;

unsigned long sensor1StartTime = 0;  // Variable to store the start time of object detection by Sensor 1

void resetFunc() {
  asm volatile("  jmp 0");
}

void setup() {
  Serial.begin(9600);         
  pinMode(TRIG_PIN1, OUTPUT);  // Sensor 1
  pinMode(ECHO_PIN1, INPUT);   // Sensor 1
  pinMode(TRIG_PIN2, OUTPUT);  // Sensor 2
  pinMode(ECHO_PIN2, INPUT);   // Sensor 2
  pinMode(SERVO_PIN, OUTPUT);  // Servo 
  pinMode(RED_LED_PIN, OUTPUT);
  servo.attach(SERVO_PIN);     
  servo.write(0);              
}

void checkSensor1Timeout() {
  // Check if Sensor 1 has been continuously detecting an object for more than 3 seconds
  if (distance_cm1 < DISTANCE_THRESHOLD) {
    if (millis() - sensor1StartTime > 5000) {
      // If detected for more than 3 seconds, restart the Arduino
      Serial.println("Object detected by Sensor 1 for more than 3 seconds. Restarting...");
      delay(5000);  // Wait for 3 seconds before restarting
      resetFunc();  // Software reset
    }
  } else {
    // Reset the start time if no object is detected
    sensor1StartTime = millis();
  }
}

void loop() {
  //Sensor 1
  digitalWrite(TRIG_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN1, LOW);
  duration_us1 = pulseIn(ECHO_PIN1, HIGH);
  distance_cm1 = 0.050 * duration_us1;

  // Check if Sensor 1 has detected an object for more than 3 seconds
  checkSensor1Timeout();

  //Sensor 2
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);
  duration_us2 = pulseIn(ECHO_PIN2, HIGH);
  distance_cm2 = 0.040 * duration_us2;

  // Servo control - Sensor 1
  if (distance_cm1 < DISTANCE_THRESHOLD && comparator != distance_cm1) {
    comparator = distance_cm1;
    for (int pos = 0; pos <= 70; pos += 1) {
      servo.write(pos);
      delay(15);
    }
    delay(2500);
    for (int pos = 70; pos >= 0; pos -= 1) {
      servo.write(pos);
      delay(15);
    }
  } else {
    servo.write(0);
  }

  // Red LED 
  if (distance_cm2 < FULL_THRESHOLD) {
    digitalWrite(RED_LED_PIN, HIGH); 
  } else {
    digitalWrite(RED_LED_PIN, LOW);   
  }

  Serial.print("Distance (Sensor 1): ");
  Serial.print(distance_cm1);
  Serial.print(" cm\t");

  Serial.print("Distance (Sensor 2): ");
  Serial.print(distance_cm2);
  Serial.print(" cm\t");

  if (distance_cm2 < FULL_THRESHOLD) {
    Serial.println("Bin Full");
  } else {
    Serial.println("Bin Not Full");
  }

  delay(1000);
}