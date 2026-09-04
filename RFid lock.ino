#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

#define RELAY_PIN 7
#define IR_PIN 6
#define BUZZER_PIN 8
#define BTN_ADD 2
#define BTN_REMOVE 3

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== DATABASE =====
String cards[] = {
  "enter card id 1",
  "enter card id 2.."
};

String names[] = {
  "name for your card 1",
  "name for your card 2"
};

int cardCount = 2;

bool addMode = false;
bool removeMode = false;

// ===== TIMING =====
unsigned long lastRFID = 0;
bool doorUnlocked = false;

// ===== UID =====
String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

// ===== FIND CARD =====
int findCard(String uid) {
  for (int i = 0; i < cardCount; i++) {
    if (cards[i] == uid) return i;
  }
  return -1;
}

// ===== BUZZER =====
void beep(int t) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(t);
  digitalWrite(BUZZER_PIN, LOW);
}

void successBeep() {
  beep(100);
}

void alarmBeep() {
  for (int i = 0; i < 3; i++) {
    beep(120);
    delay(80);
  }
}

// ===== RFID =====
void handleRFID() {

  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uid = getUID();
  int index = findCard(uid);

  lcd.clear();
  lcd.print("Scanning...");
  delay(300);

  if (addMode) {
    cards[cardCount] = uid;
    names[cardCount] = "User" + String(cardCount);
    cardCount++;

    lcd.clear();
    lcd.print("Card Added");
    successBeep();
    addMode = false;
  }

  else if (removeMode) {
    int pos = findCard(uid);

    if (pos != -1) {
      for (int i = pos; i < cardCount - 1; i++) {
        cards[i] = cards[i + 1];
        names[i] = names[i + 1];
      }
      cardCount--;

      lcd.clear();
      lcd.print("Removed");
      successBeep();
    }

    removeMode = false;
  }

  else {
    if (index != -1) {
      lcd.clear();
      lcd.print("Welcome");
      lcd.setCursor(0, 1);
      lcd.print(names[index]);

      successBeep();

      digitalWrite(RELAY_PIN, HIGH); // 🔓 UNLOCK
      doorUnlocked = true;
    }
    else {
      lcd.clear();
      lcd.print("ACCESS DENIED");
      alarmBeep();
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_ADD, INPUT_PULLUP);
  pinMode(BTN_REMOVE, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, LOW);

  lcd.print("Smart Lock Ready");
  delay(1500);
  lcd.clear();
}

// ===== LOOP =====
void loop() {

  // BUTTONS
  if (digitalRead(BTN_ADD) == LOW) {
    addMode = true;
    removeMode = false;
    lcd.clear();
    lcd.print("ADD MODE");
    delay(300);
  }

  if (digitalRead(BTN_REMOVE) == LOW) {
    removeMode = true;
    addMode = false;
    lcd.clear();
    lcd.print("REMOVE MODE");
    delay(300);
  }

  // 🚨 IR SENSOR = ONLY LOCK TRIGGER
  static bool lastIRState = HIGH;
  bool irState = digitalRead(IR_PIN);

  if (irState == LOW && lastIRState == HIGH) {
    // ONLY ON NEW DETECTION EDGE
    if (doorUnlocked) {
      digitalWrite(RELAY_PIN, LOW); // 🔒 LOCK
      lcd.clear();
      lcd.print("Door Locked");
      beep(300);
      doorUnlocked = false;
    }
  }

  lastIRState = irState;

  // RFID
  if (millis() - lastRFID > 150) {
    handleRFID();
    lastRFID = millis();
  }
}