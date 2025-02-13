#include <Arduino.h>
#include <ESP32Servo.h>

#define PHOTORESISTOR_PIN 12
#define LED_PIN 27
#define SERVO_PIN 26

uint16_t maxLightVal = 0;
uint16_t minLightVal = 0xFFFF;

Servo myservo;

void calibrate() {
  Serial.println("Calibrating...");
  
  const int sampleDelayMs = 1000 / 10;
  const int calibrationTimeMs = 10 * 1000;
  
  uint16_t sampleVal;
  bool ledOn = true;
  for (int i = 0; i < calibrationTimeMs; i += sampleDelayMs) {
    uint16_t sampleVal = analogRead(PHOTORESISTOR_PIN);

    if (sampleVal < minLightVal) {
      minLightVal = sampleVal;
    }

    if (sampleVal > maxLightVal) {
      maxLightVal = sampleVal;
    }
    
    // Write current LED state
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);

    // Flip the LED state every 200ms
    if (i % 200 == 0) {
      ledOn = !ledOn;
    }

    // Wait until next sampling round
    delay(sampleDelayMs);
  }

  // Turn off the LED when we're done
  digitalWrite(LED_PIN, LOW);

  // Print stats
  Serial.printf("Done calibrating. max = %d, min = %d\n", maxLightVal, minLightVal);
}

void setup() {
  // Start the serial port
  Serial.begin(9600);

  // Reading photoresistor value from ADC
  pinMode(PHOTORESISTOR_PIN, INPUT);

  // LED blinker
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Servo signal pin
  pinMode(SERVO_PIN, OUTPUT);
  myservo.attach(SERVO_PIN);

  // Calibrate the photoresistor sensor
  calibrate();
}

void loop() {
  // Take a sample from the photoresistor
  uint16_t lightVal = analogRead(PHOTORESISTOR_PIN);

  // Normalize it to [0..1]
  double normalizedVal = double(lightVal - minLightVal) / double(maxLightVal - minLightVal);
  if (normalizedVal < 0) {
    normalizedVal = 0;
  } else if (normalizedVal > 1) {
    normalizedVal = 1;
  }

  // Write it to the servo
  const double maxDegrees = 179.0;
  int degrees = int(normalizedVal * maxDegrees);
  myservo.write(degrees);
}
