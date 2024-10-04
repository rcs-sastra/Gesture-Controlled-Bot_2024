#include <ESP8266WiFi.h>

// Set these to your desired credentials
const char *ssid = "ESP8266_AP";
const char *password = "123456789";

// Motor control pins (use valid GPIO pins)
const int motorPin1 = D1; // GPIO5
const int motorPin2 = D2; // GPIO4
const int motorPin3 = D3; // GPIO0
const int motorPin4 = D4; // GPIO2 (also onboard LED)

// Ultrasonic sensor pins
const int trigpin = D6;  // GPIO12
const int echopin = D7;  // GPIO13

// LED pin (replaced D8 with GPIO2)
const int ledpin = D5;  // GPIO2 onboard LED

int duration;
int distance;

const int threshold_distance = 21;

WiFiServer server(12345); // Changed port number

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize motor and sensor pins
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  pinMode(ledpin, OUTPUT);

  // Set up the access point
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");

  // Print the IP address
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());

  // Start the server
  server.begin();
}

void loop() {
  
  // Check for client connection
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    while (client.connected()) {
      if (client.available()) {
        String message = client.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(message);
        client.print(message); // Echo the message back to the client

        // Object detection, stop car and turn on LED
    // Measure distance using ultrasonic sensor
    digitalWrite(trigpin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigpin, HIGH);
   delayMicroseconds(10);
    digitalWrite(trigpin, LOW);
   duration = pulseIn(echopin, HIGH);
   distance = duration * 0.0343 / 2;
   Serial.print("Distance: ");
   Serial.print(distance);
   Serial.println(" cm");

       
          // Turn off LED and perform motor movements based on command
          digitalWrite(ledpin, LOW);
          Serial.print("No object, received command: ");
          Serial.println(message);
          if (message == "1") {
             if (distance <= threshold_distance) {
          digitalWrite(ledpin, HIGH);
          Serial.println("Object detected, stopping car...");
          stopCar();
        }
           else
           {Serial.println("Moving forward");
            moveForward();
           }
          } else if (message == "2") {
               Serial.println("Moving backward");
            moveBackward();
          }
          else if (message == "3") {
             if (distance <= threshold_distance) {
          digitalWrite(ledpin, HIGH);
          Serial.println("Object detected, stopping car...");
          stopCar();
        }
           else{
            Serial.println("Turning left");
            turnLeft();}
            
          } else if (message == "4") {
             if (distance <= threshold_distance) {
          digitalWrite(ledpin, HIGH);
          Serial.println("Object detected, stopping car...");
          stopCar();
        }
          else { Serial.println("Turning right");
            turnRight();}
          } else if (message == "0") {
            Serial.println("Stopping car");
            stopCar();
          }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}

// Motor control functions
void turnLeft() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  Serial.println("Turning left");
}

void turnRight() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  Serial.println("Turning right");
}

void moveForward() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  digitalWrite(motorPin3, HIGH);
  digitalWrite(motorPin4, LOW);
  Serial.println("Moving forward");
}

void moveBackward() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, HIGH);
  Serial.println("Moving backward");
}

void stopCar() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
  digitalWrite(motorPin3, LOW);
  digitalWrite(motorPin4, LOW);
  Serial.println("Car stopped");
}
