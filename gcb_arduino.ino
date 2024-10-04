#include <ESP8266WiFi.h>

// Object Detection and alerting:
const int trigPin = 18;
const int echoPin = 5;
const int buzzerPin = 33;
long duration = 0;
int distance = 0;
const int threshold_distance = 7;
// Led Pins:
const int ledPin_Object = 25;
const int ledPin1 = 26;
const int ledPin2 = 27;
const int ledPin3 = 14;
const int ledPin4 = 12;

// Set these to your desired credentials
const char *ssid = "ESP32_AP";
const char *password = "123456789";
const int motorPin1 = 35;
const int motorPin2 = 32;
const int motorPin3 = 21;
const int motorPin4 = 19;
WiFiServer server(12345);

unsigned long lastDistanceCheck = 0;
const unsigned long distanceCheckInterval = 100; // Check distance every 100ms

bool obstacleDetected = false;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  pinMode(ledPin_Object, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.0344 / 2;
}

void handleObstacle() {
  obstacleDetected = true;
  digitalWrite(ledPin_Object, HIGH);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  digitalWrite(ledPin_Object, LOW);
  digitalWrite(buzzerPin, LOW);
}

void controlMovement(int command) {
  if (obstacleDetected && command == 1) {
    // If obstacle detected and trying to move forward, stop instead
    command = 0;
  }

  switch (command) {
    case 1: // Forward
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin3, HIGH);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin4, LOW);
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
      break;
    case 2: // Reverse
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, HIGH);
      break;
    case 3: // Right
      digitalWrite(ledPin1, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin4, LOW);
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
      break;
    case 4: // Left
      digitalWrite(ledPin3, HIGH);
      digitalWrite(ledPin4, HIGH);
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, HIGH);
      break;
    default: // Stop
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
      digitalWrite(ledPin1, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
      digitalWrite(ledPin4, LOW);
      break;
  }
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("Client connected");
    while (client.connected()) {
      unsigned long currentMillis = millis();
      
      // Check distance periodically
      if (currentMillis - lastDistanceCheck >= distanceCheckInterval) {
        distance = measureDistance();
        lastDistanceCheck = currentMillis;
        
        if (distance < threshold_distance) {
          handleObstacle();
        } else {
          obstacleDetected = false;
        }
      }
      
      if (client.available()) {
        String message = client.readStringUntil('\n');
        Serial.print("Received: ");
        Serial.println(message);
        
        int command = message.toInt();
        controlMovement(command);
        
        // Send distance and obstacle status back to the client
        String response = String(distance) + "," + String(obstacleDetected) + "\n";
        client.print(response);
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}