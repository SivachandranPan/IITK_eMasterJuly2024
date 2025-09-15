#include <LiquidCrystal_I2C.h>

// Initializing the LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Defining pins for the temperature sensor and its LED
#define TEMP_SENSOR A5
#define TEMP_LED 3

// Defining pins for the motion sensor and its LED
#define MOTION_SENSOR 5
#define MOTION_LED 6

// Defining pins for the ultrasonic sensor and its LED
#define ULTRASONIC_SENSOR_ECHO 2
#define ULTRASONIC_SENSOR_TRIG 9
#define ULTRASONIC_LED 10

// Variables to monitor motion detection
bool motionDetected = false;
int motionStatus = 0;
unsigned long motionTimer = 0;

void setup() {
  // Initialize LCD screen
  lcd.init();
  lcd.backlight();

  // Start serial communication
  Serial.begin(9600);

  // Display initial message on display
  lcd.setCursor(2, 2);
  lcd.print("Welcome to Rita's Smart Home");

  // Set pin modes
  pinMode(TEMP_SENSOR, INPUT);
  pinMode(TEMP_LED, OUTPUT);
  pinMode(MOTION_SENSOR, INPUT);
  pinMode(MOTION_LED, OUTPUT);
  pinMode(ULTRASONIC_SENSOR_TRIG, OUTPUT);
  pinMode(ULTRASONIC_SENSOR_ECHO, INPUT);
  pinMode(ULTRASONIC_LED, OUTPUT);
}

void loop() {
  // Wait for 2 seconds
  delay(2000);
  lcd.clear(); // Clear LCD screen before updating

  // Read data from sensors
  senseTemperature();
  senseMotion();
  senseUltrasonicDistance();
}

/**
 * NTC Thermistor-Based Temperature Monitoring System
 * 
 * Functionality:
 * - Reads the temperature using an NTC thermistor and converts it into Celsius.
 * - Displays real-time temperature on an LCD screen.
 * - Controls an LED indicator to simulate a smart cooling and heating system:
 *     - Temperature < 19°C → Heating LED ON
 *     - Temperature > 24°C → Cooling LED ON
 *     - 19°C ≤ Temperature ≤ 24°C → System remains stable, LED OFF
 * 
 * Components:
 * - NTC Thermistor (Connected to TEMP_SENSOR pin)
 * - LED Indicator (Connected to TEMP_LED pin)
 * - LCD Display (Shows temperature and system status)
 * 
 * Expected Behavior:
 * - Displays temperature in real-time.
 * - Activates respective LEDs based on temperature thresholds.
 */

// Function to monitor temperature with Smart on/off cooler and heater LED alert
void senseTemperature() {
  // Read analog value from temperature sensor
  int tempSensorValue = analogRead(TEMP_SENSOR);

  // Convert sensor value to temperature in Celsius
  float temperature = 1 / (log(1 / (1023.0 / tempSensorValue - 1)) / 3950 + 1.0 / 298.15) - 273.15;

  // Convert temperature to a string
  char tempStr[6];
  dtostrf(temperature, 4, 1, tempStr);

  // Display temperature on display
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempStr);
  lcd.print(" *C  ");

  // Display heating or cooling status based on temperature range
  lcd.setCursor(0, 1);
  if (temperature < 19) {
    digitalWrite(TEMP_LED, HIGH); // Turn on heating LED
    lcd.print("Heating: ON    ");
  } else if (temperature > 24) {
    digitalWrite(TEMP_LED, HIGH); // Turn on cooling LED
    lcd.print("Cooling: ON    ");
  } else {
    digitalWrite(TEMP_LED, LOW); // Turn off LED when temperature is stable
    lcd.print("Stable Temp    ");
  }
}

/**
 * PIR (Passive Infrared) Motion Sensor System
 * 
 * Functionality:
 * - Detects motion when motion is simulated, the sensor outputs HIGH and activates the LED.
 * - The LED remains ON for 5 seconds.
 * - After 5 seconds, the sensor resets and enters an inhibit period of 1.2 seconds, 
 *   during which it ignores further motion.
 * - Motion status is displayed on the LCD and logged in the Serial Monitor.
 * 
 * Components:
 * - PIR Motion Sensor (OUT pin connected to MOTION_SENSOR pin)
 * - LED Indicator (Connected to MOTION_LED pin)
 * - LCD Display (Shows motion detection status)
 * 
 * Expected Behavior:
 * - Motion detected → LED ON, LCD updates, Serial prints "Motion detected!"
 * - After 5 seconds → LED OFF, LCD updates, Serial prints "Motion ended!"
 * - 1.2 seconds inhibit time before detecting new motion.
 */

// Function to detect motion with timed LED activation and inhibit time
void senseMotion() {
  // Read motion sensor value (HIGH if motion detected)
  motionStatus = digitalRead(MOTION_SENSOR);

  // If motion is detected for the first time
  if (motionStatus == HIGH && !motionDetected) {
    digitalWrite(MOTION_LED, HIGH); // Turn on motion LED
    motionDetected = true;
    motionTimer = millis(); // Save the current time

    // Display motion status on dislay
    lcd.setCursor(0, 2);
    lcd.print("Motion: Detected ");
    Serial.println("Motion detected!");
  }

  // If motion was detected but 5 seconds have passed, reset motion status
  if (motionDetected && millis() - motionTimer > 5000) {
    motionDetected = false;
    digitalWrite(MOTION_LED, LOW); // Turn off motion LED

    // Display no motion status on LCD
    lcd.setCursor(0, 2);
    lcd.print("Motion: None     ");
    Serial.println("Motion ended!");
  }

  // Add 1.2s inhibit time before detecting motion again
  delay(1200);
}

/**
 * Ultrasonic Distance Measurement System
 * 
 * Functionality:
 * - Uses an ultrasonic sensor to measure the distance of objects.
 * - Sends an ultrasonic pulse and calculates the time taken for the echo to return.
 * - Converts the duration into a distance measurement in centimeters.
 * - Displays the measured distance on an LCD.
 * - Includes a random deviation of 5-12 cm to simulate real-world conditions.
 * - Activates an LED alert when an object is detected within 15 cm.
 * 
 * Components:
 * - Ultrasonic Sensor (Trigger & Echo pins: ULTRASONIC_SENSOR_TRIG, ULTRASONIC_SENSOR_ECHO)
 * - LED Indicator (Connected to ULTRASONIC_LED pin)
 * - LCD Display (Shows measured distance)
 * 
 * Expected Behavior:
 * - Displays real-time distance on the LCD.
 * - If an object is closer than 15 cm → LED ON.
 * - If an object is farther than 15 cm → LED OFF.
 * - Logs measurements to the Serial Monitor.
 */

// Function to measure distance using an ultrasonic sensor
void senseUltrasonicDistance() {
  // Send a short pulse to trigger the ultrasonic sensor
  digitalWrite(ULTRASONIC_SENSOR_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_SENSOR_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_SENSOR_TRIG, LOW);

  // Measure the time taken for the echo pulse to return
  long duration = pulseIn(ULTRASONIC_SENSOR_ECHO, HIGH);

  // Convert time to distance (cm)
  int distance = duration * 0.034 / 2;

  // Apply a random deviation of 5-12 cm to simulate real-world noise
  int adjustedDistance = distance + random(5, 13);

  // Display distance on LCD
  lcd.setCursor(0, 3);
  lcd.print("Dist: ");
  lcd.print(adjustedDistance);
  lcd.print(" cm ");

  // If distance is less than 15 cm, turn on LED
  if (adjustedDistance < 15) {
    digitalWrite(ULTRASONIC_LED, HIGH);
  } else {
    digitalWrite(ULTRASONIC_LED, LOW);
  }

  // Print distance values to Serial Monitor for debugging
  Serial.print("Measured Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Adjusted Distance: ");
  Serial.print(adjustedDistance);
  Serial.println(" cm");
}
