#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define PASSWORD_LENGTH 5
#define MAX_ATTEMPTS 3

Servo myservo;
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

char inputData[PASSWORD_LENGTH];
char masterPassword[PASSWORD_LENGTH] = "1234";
byte dataCount = 0;
byte failedAttempts = 0;

bool isDoorOpen = false;
bool isLockedOut = false;
char keyPressed;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  myservo.attach(9, 500, 2500);
  closeDoor();

  lcd.begin(16, 2);
  lcd.print("Protected Door");
  showLoading("Initializing");
  lcd.clear();
}

void loop() {
  if (isLockedOut) {
    keyPressed = keypad.getKey();
    lcd.setCursor(0, 0);
    lcd.print(" SYSTEM LOCKED ");
    lcd.setCursor(0, 1);
    lcd.print("Press D to reset");

    if (keyPressed == 'D') {
      isLockedOut = false;
      failedAttempts = 0;
      lcd.clear();
      lcd.print("System Reset");
      delay(2000);
      lcd.clear();
    }
    return;
  }

  keyPressed = keypad.getKey();

  if (isDoorOpen) {
    if (keyPressed == '#') {
      lcd.clear();
      closeDoor();
      lcd.print("Door Closed");
      delay(2000);
      lcd.clear();
      isDoorOpen = false;
    }
  } else {
    handlePasswordInput();
  }
}

void showLoading(const char* msg) {
  lcd.setCursor(0, 1);
  lcd.print(msg);
  for (int i = 0; i < 6; i++) {
    delay(500);
    lcd.print(".");
  }
  delay(500);
}

void clearInput() {
  memset(inputData, 0, sizeof(inputData));
  dataCount = 0;
}

void closeDoor() {
  for (int pos = 90; pos >= 0; pos -= 5) {
    myservo.write(pos);
    delay(15);
  }
}

void openDoor() {
  for (int pos = 0; pos <= 90; pos += 5) {
    myservo.write(pos);
    delay(15);
  }
}

void handlePasswordInput() {
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  if (keyPressed) {
    if (keyPressed == '*') {
      clearInput();
      lcd.setCursor(0, 1);
      lcd.print("                ");
    } else {
      if (dataCount < PASSWORD_LENGTH - 1) {
        inputData[dataCount++] = keyPressed;
        lcd.setCursor(dataCount - 1, 1);
        lcd.print('*');
      }

      if (dataCount == PASSWORD_LENGTH - 1) {
        lcd.clear();
        if (strcmp(inputData, masterPassword) == 0) {
          openDoor();
          lcd.print("Access Granted");
          isDoorOpen = true;
          failedAttempts = 0;
          delay(5000);

          lcd.clear();
          lcd.print("Closing Door...");
          closeDoor();
          isDoorOpen = false;
          delay(2000);
        } else {
          failedAttempts++;
          lcd.print("Access Denied");
          delay(2000);

          if (failedAttempts >= MAX_ATTEMPTS) {
            lcd.clear();
            lcd.print(" SYSTEM LOCKED ");
            delay(2000);
            isLockedOut = true;
          }
        }
        clearInput();
        lcd.clear();
      }
    }
  }
}
