
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

// Parametr maksymalnej mocy silników (0.0-1.0)
// Zmniejsz tę wartość aby ograniczyć maksymalną prędkość
// #define MAX_MOTOR_POWER 1.0  // Zakres: 0.0-1.0 (1.0 = pełna moc, 0.5 = połowa mocy)
// #define MAX_MOTOR_POWER 0.8   // 80% mocy  
// #define MAX_MOTOR_POWER 0.6   // 60% mocy
#define MAX_MOTOR_POWER 0.5   // 50% mocy (połowa prędkości)
// #define MAX_MOTOR_POWER 0.3   // 30% mocy (tryb powolny)
// #define MAX_MOTOR_POWER 0.1   // 10% mocy (tryb bardzo powolny)

// Podfunkcje sterowania motorami
void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  // Ograniczenie wartości do zakresu 0-255 (konwersja z 0.0-1.0)
  int maxPwm = MAX_MOTOR_POWER * 255;
  if (leftSpeed > maxPwm) leftSpeed = maxPwm;
  if (leftSpeed < 0) leftSpeed = 0;
  if (rightSpeed > maxPwm) rightSpeed = maxPwm;
  if (rightSpeed < 0) rightSpeed = 0;
  
  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void driveForward(float speed, float turn) {
  forward();
  int baseSpeed = (MAX_MOTOR_POWER * 255) * speed;
  
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
  int baseSpeed = (MAX_MOTOR_POWER * 255) * speed;
  
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
    setMotorSpeeds(0, (MAX_MOTOR_POWER * 255) * turnValue);
  } else {
    // Obrót w lewo - prawy silnik do przodu, lewy stop
    setMotorSpeeds((MAX_MOTOR_POWER * 255) * (-turnValue), 0);
  }
}

void stopMotors() {
  setMotorSpeeds(0, 0);
}

// Funkcja do obliczania płynnego sterowania wektorowego
void vectorDriveMotors(float xVal, float yVal) {
  // Obliczenie promienia (siły) bez sqrt - używamy magnitude^2 dla porównania
  float magnitudeSquared = xVal * xVal + yVal * yVal;
  
  // Definicja martwej strefy (squared dla porównania)
  const float deadZoneSquared = 0.15 * 0.15;
  
  if (magnitudeSquared < deadZoneSquared) {
    // Joystick w martwej strefie - stop
    stopMotors();
    return;
  }
  
  // Obliczenie rzeczywistej magnitude
  float magnitude = magnitudeSquared; // Aproksymacja dla uproszczenia
  if (magnitude > 1.0) magnitude = 1.0;
  
  // Obliczenie prędkości dla każdego silnika na podstawie wektora
  float leftMotorSpeed = 0;
  float rightMotorSpeed = 0;
  
  // Algorytm miksowania - płynne przejście między ruchem a obrotem
  // yVal kontroluje ruch do przodu/tyłu, xVal kontroluje skręcanie
  
  // Składowa ruchu do przodu/tyłu
  float forwardComponent = yVal;
  
  // Składowa skręcania (różnicowa prędkość kół)
  float turnComponent = xVal;
  
  // Obliczenie prędkości każdego silnika
  leftMotorSpeed = forwardComponent + turnComponent;
  rightMotorSpeed = forwardComponent - turnComponent;
  
  // Ograniczenie do zakresu -1.0 do 1.0
  if (leftMotorSpeed > 1.0) leftMotorSpeed = 1.0;
  if (leftMotorSpeed < -1.0) leftMotorSpeed = -1.0;
  if (rightMotorSpeed > 1.0) rightMotorSpeed = 1.0;
  if (rightMotorSpeed < -1.0) rightMotorSpeed = -1.0;
  
  // Konwersja na wartości PWM i ustawienie kierunku
  setMotorWithDirection(ENA, leftMotorSpeed);
  setMotorWithDirection(ENB, rightMotorSpeed);
}

// Funkcja do ustawiania silnika z kierunkiem (do przodu/tyłu)
void setMotorWithDirection(int enablePin, float speed) {
  int pwmValue = (speed >= 0) ? (speed * MAX_MOTOR_POWER * 255) : (-speed * MAX_MOTOR_POWER * 255);
  
  if (enablePin == ENA) {
    // Lewy silnik (IN1, IN2)
    if (speed > 0) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    } else if (speed < 0) {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    } else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
    }
  } else if (enablePin == ENB) {
    // Prawy silnik (IN3, IN4)
    if (speed > 0) {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    } else if (speed < 0) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    } else {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
  }
  
  analogWrite(enablePin, pwmValue);
}

void driveMotors(float xVal, float yVal){
  // Używamy nowego płynnego sterowania wektorowego
  vectorDriveMotors(xVal, yVal);
}

void setup() {
  // Inicjalizacja komunikacji szeregowej
  Serial.begin(9600);
  Serial.println("L298N Motor Controller - Start");

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

  // Logowanie wartości analogowych do portu szeregowego
  Serial.print("X: ");
  Serial.print(xVal, 3);  // 3 cyfry po przecinku
  Serial.print(" | Y: ");
  Serial.print(yVal, 3);
  Serial.print(" | SW: ");
  Serial.print(sVal);
  Serial.println();  // Nowa linia
  
  // Opóźnienie dla czytelności logów (opcjonalne)
  delay(300);  // Odkomentuj jeśli logi są za szybkie

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
