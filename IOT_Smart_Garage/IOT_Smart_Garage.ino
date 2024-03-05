//--------------------------------------- some definations -------------------------------------------------------------------------------------------------------------------------------------------------
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "Amy Morsi";
const char *password = "12345678";

#define trigPin 23  
#define echoPin 22  

#define LED1 18
#define LED2 19

const int irSensorPin1 = 3; 
const int irSensorPin2 = 5; 

const int irSensorPin3 = 4; 

const int servoPin = 21;

AsyncWebServer server(80);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  
  pinMode(irSensorPin1, INPUT);
  pinMode(irSensorPin2, INPUT);
  pinMode(irSensorPin3, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // wifi setup
  connectToWiFi();
  // webpage setup
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><body>";
    html += "<h1>Smart Parking System</h1>";
    html += "<p>Parking Area 1 : " + getStatus(irSensorPin1) + "</p>";
    html += "<p>Parking Area 2 : " + getStatus(irSensorPin2) + "</p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.begin();
}

void loop() {
//-----------------------------------------Ultrasonic---------------------------------------------------------------------------------------------------------//
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo duration
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  // Using a float to get more accurate distance values
  float distance = (duration / 2.0) * 0.0343;

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
//-------------------------------------------------------------IR Sensors--------------------------------------------------------------------------------------//
  int irSensorValue1 = digitalRead(irSensorPin1);
  int irSensorValue2 = digitalRead(irSensorPin2);
  int irSensorValue3 = digitalRead(irSensorPin3);

  if (irSensorValue1 == HIGH) {
    Serial.println("IR 1 : No Obstacle detected!");
    digitalWrite(LED1,HIGH);
  } else {
    Serial.println("IR 1 : Obstacle detected.");
    digitalWrite(LED1,LOW);
  }
  if (irSensorValue2 == HIGH) {
    Serial.println("IR 2 : No Obstacle detected!");
    digitalWrite(LED2,HIGH);
  } else {
    Serial.println("IR 2 : Obstacle detected.");
    digitalWrite(LED2,LOW);
  }
    if (irSensorValue3 == HIGH) {
    Serial.println("IR 3 : No Obstacle detected!");
  } else {
    Serial.println("IR 3 : Obstacle detected.");
  }
//----------------------------------------------------------------------------------------------------------------------------------------------------------//
  // The following part checks if the distance is less than 10 cm or the ir sensor detects a car
  // when a car enters (and there is avilable parking area) or leaves
  if (distance < 10 | irSensorValue3 == LOW) {
    if(irSensorValue1 == HIGH |irSensorValue2 == HIGH){
      moveServo(90);
    Serial.println("open gate");
    delay(1000); // Wait for 1 second
    }
  }
  else
  {
    //close gate
    moveServo(0);
  }
  delay(500); 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void moveServo(int angle) {
  // Convert angle to pulse width (500 to 2400 microseconds)
  int pulseWidth = map(angle, 0, 180, 500, 2400);

  // Generate PWM signal on servo pin
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(servoPin, LOW);

  // Pause for the servo to reach the desired position
  //delay(20);
}
void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

String getStatus(int irSensorPin) {
  return digitalRead(irSensorPin) == LOW ? "Occupied" : "Empty";
}
