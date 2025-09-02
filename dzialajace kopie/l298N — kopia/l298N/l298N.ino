
// Motor Connections (ENA & ENB must use PWM pins)
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6
#define ENA 10
#define ENB 5
#define XPIN A0
#define YPIN A1
#define SWPIN A2

// Podfunkcje sterowania motorami
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Ograniczenie wartości do zakresu 0-255
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);
  
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void driveForward(float speed, float turn) {
  forward();
  int baseSpeed = 255 * speed;
  
  if (turn > 0) {
    // Skręt w prawo - zmniejsz prędkość prawego silnika
    setMotorSpeeds(baseSpeed, baseSpeed * (1 - turn));
  } else if (turn < 0) {
    // Skręt w lewo - zmniejsz prędkość lewego silnika
    setMotorSpeeds(baseSpeed * (1 + turn), baseSpeed);
  } else {
    // Jazda prosto
    setMotorSpeeds(baseSpeed, baseSpeed);
  }
}

void driveReverse(float speed, float turn) {
  reverse();
  int baseSpeed = 255 * speed;
  
  if (turn > 0) {
    // Skręt w prawo podczas cofania
    setMotorSpeeds(baseSpeed, baseSpeed * (1 - turn));
  } else if (turn < 0) {
    // Skręt w lewo podczas cofania
    setMotorSpeeds(baseSpeed * (1 + turn), baseSpeed);
  } else {
    // Cofanie prosto
    setMotorSpeeds(baseSpeed, baseSpeed);
  }
}

void turnInPlace(float turnValue) {
  forward(); // Kierunek silników do obrotu w miejscu
  
  if (turnValue > 0) {
    // Obrót w prawo - lewy silnik do przodu, prawy stop
    setMotorSpeeds(0, 255 * turnValue);
  } else {
    // Obrót w lewo - prawy silnik do przodu, lewy stop
    setMotorSpeeds(255 * (-turnValue), 0);
  }
}

void stopMotors() {
  setMotorSpeeds(0, 0);
}

void driveMotors(float xVal, float yVal){
  // Definicja martwej strefy joysticka
  const float deadZone = 0.2;
  
  if (yVal > deadZone) {
    // Jazda do przodu
    driveForward(yVal, xVal);
  }
  else if (yVal < -deadZone) {
    // Jazda do tyłu
    driveReverse(-yVal, xVal);
  }
  else if (xVal > deadZone || xVal < -deadZone) {
    // Obrót w miejscu (gdy joystick jest w pozycji środkowej w osi Y)
    turnInPlace(xVal);
  }
  else {
    // Zatrzymanie motorów (joystick w pozycji środkowej)
    stopMotors();
  }
}

void setup() {

  // Set motor connections as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(XPIN, INPUT);
  pinMode(YPIN, INPUT);
  pinMode(SWPIN, INPUT_PULLUP);

  // Start with motors off
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// bool toggle = false;

void loop() {

  float xVal = (analogRead(XPIN) - 512.0) / 512.0;  // Normalizacja do zakresu -1.0 do +1.0
  float yVal = (analogRead(YPIN) - 512.0) / 512.0;  // Normalizacja do zakresu -1.0 do +1.0
  int sVal = digitalRead(SWPIN);

  // if (toggle == false)
  //   driveMotors(xVal, yVal);
  // else {
  
  // }
  
  // Aktywne sterowanie motorami
  driveMotors(xVal, yVal);  
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
