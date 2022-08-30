#include <SoftwareSerial.h>

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>  //this library is included in the Arduino IDE
#include <Adafruit_Fingerprint.h>
#define push_input A5

SoftwareSerial mySerial(9, 10);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal lcd(2, 3, 4, 11, 12, 7);
SoftwareSerial Arduino_SoftSerial (6, 5); // rx , tx
int cand1 = 0 , cand2 = 0 , cand3 = 0 , cand4 = 0;

int Reset  = A0;
int Candidate_A = A1;
int Candidate_B = A2;
int Candidate_C = A3;
int Candidate_D = A4;

int pushButton, j, flag, start, timer;
int buz = 13;
int led = 8;
int disp = 1;
int flag_reset = 0;

void setup() {

  pinMode(Reset, INPUT_PULLUP);
  pinMode(push_input, INPUT_PULLUP);

  pinMode(Candidate_A, INPUT);
  pinMode(Candidate_B, INPUT);
  pinMode(Candidate_C, INPUT);
  pinMode(Candidate_D, INPUT);

  pinMode(buz, OUTPUT);
  pinMode(led, OUTPUT);

  //  lcd.begin(20, 4);
  //  lcd.clear();
  //  lcd.setCursor(0, 0);
  //  lcd.print("Smart  Electronic");
  //  lcd.setCursor(0, 1);
  //  lcd.print("Voting Machine");
  //  delay(2000);
  //  lcd.clear();

  welcome();

  Serial.begin(19200); // ....................update korsi
  Serial.println("\n\nAdafruit finger detect test");
  Arduino_SoftSerial.begin(9600);
  //displaySetup();

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

}

void loop() {

  while (1)
  {
    switch_press();
    if (digitalRead(push_input) == 0)
    {
      break;
    }
    reset_system();
  }

  while (1)
  {
    put_finger();
    flag = 0;
    getFingerprintID();
    delay(50);
    if (flag > 0)
    {
      break;
    }
    reset_system();

  }

  while (1)
  {
    found_id_place_vote();
    delay(1000);


    if ((flag == 0) | Read_a_Voter(flag + 100) == 1) {
      if (Read_a_Voter(flag + 100) == 1)
      {
        while (j <= 5)
        {

          Alarm();
          lcd.begin(16, 2);
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print("False    Voter");
   

          j += 1;
          delay(800);
        }
        j = 0;
      }
      pushButton = 1;

      break;

    }
    else {

      pushButton = 0;
      Write_a_Voter(flag + 100);
      break;
    }
    reset_system();
  }
   
  if (pushButton == 0) {
    start = 1;
    digitalWrite(buz, HIGH);
    delay(500);
    digitalWrite(buz, LOW);
  }

 while (!pushButton)
  {
    if (start == 1) {
      digitalWrite(led, HIGH);
    }


    if (digitalRead (Candidate_A) == 1 && start == 1)
    {
      start = 0;
      Count_Vote(1, 0, 0, 0);
      Alarm();
      lcd.begin(20, 4);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your    vote : A ");
      delay(3000);
      break;
    }

    if (digitalRead (Candidate_B) == 1 && start == 1)
    {
      start = 0;
      Count_Vote(0, 1, 0, 0);
      Alarm();
      lcd.begin(20, 4);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your    vote : B ");
      delay(3000);
      break;

    }

    if (digitalRead (Candidate_C) == 1 && start == 1) {
      start = 0;
      Count_Vote(0, 0, 1, 0);
      Alarm();
      lcd.begin(20, 4);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your    vote : C ");
      delay(3000);
      break;

    }

    if (digitalRead (Candidate_D) == 1 && start == 1) {
      start = 0;
      Count_Vote(0, 0, 0, 1);
      Alarm();
      lcd.begin(20, 4);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your    vote : D ");
      delay(3000);
      break;

    }
  }
  if (start == 0) {
    digitalWrite(led, LOW);
  }

  






  while (1)
  {
    
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("A=");
    lcd.print(EEPROM.read(1));
    //lcd.setCursor(0,1);
    lcd.print("    B=");
    lcd.print(EEPROM.read(2));
    lcd.setCursor(0, 1);
    lcd.print("C=");
    lcd.print(EEPROM.read(3));
    //lcd.setCursor(0,3);
    lcd.print("    D=");
    lcd.print(EEPROM.read(4));

    delay(2000);
    reset_system();
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("Switch  OFF");
    lcd.setCursor(0, 1);
    lcd.print("Fingrprint");
    delay(2000);
    lcd.clear();
    if (digitalRead(push_input) == 1)
    {
      break;
    }
    reset_system();
    

  }

  cand1 = EEPROM.read(1);
  cand2 = EEPROM.read(2);
  cand3 = EEPROM.read(3);
  cand4 = EEPROM.read(4);




  Arduino_SoftSerial.print(cand1);
  Arduino_SoftSerial.print("A");

  Arduino_SoftSerial.print(cand2);
  Arduino_SoftSerial.print("B");

  Arduino_SoftSerial.print(cand3);
  Arduino_SoftSerial.print("C");

  Arduino_SoftSerial.print(cand4);
  Arduino_SoftSerial.print("D");

  delay(500);

}

uint8_t getFingerprintID()
{
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  flag = finger.fingerID;
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

void Alarm() {
  digitalWrite(buz, HIGH);
  delay(200);
  digitalWrite(buz, LOW);
  delay(100);
  digitalWrite(buz, HIGH);
  delay(200);
  digitalWrite(buz, LOW);
  delay(100);
  digitalWrite(buz, HIGH);
  delay(200);
  digitalWrite(buz, LOW);

}
void Write_a_Voter(int i)
{
  EEPROM.update(i, 1);
}
int Read_a_Voter(int i)
{
  return EEPROM.read(i);
}


void Count_Vote(int k, int l, int m, int n)
{
  if (k == 1) {
    int Candidate_A = EEPROM.read(1);
    Candidate_A += 1;
    EEPROM.update(1, Candidate_A);
  }
  if (l == 1) {
    int Candidate_B = EEPROM.read(2);
    Candidate_B += 1;
    EEPROM.update(2, Candidate_B);
  }
  if (m == 1) {
    int Candidate_C = EEPROM.read(3);
    Candidate_C += 1;
    EEPROM.update(3, Candidate_C);
  }
  if (n == 1) {
    int Candidate_D = EEPROM.read(4);
    Candidate_D += 1;
    EEPROM.update(4, Candidate_D);
  }
}

void welcome()
{
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to EVM");
  //lcd.setCursor(0, 1);
  //lcd.print("Machine");
  delay(1000);
  lcd.clear();
}

void switch_press()
{
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press the Switch");
  lcd.setCursor(0, 1);
  lcd.print("Below");
  delay(1000);
  lcd.clear();
}

void put_finger()
{
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place the finger");
  lcd.setCursor(0, 1);
  lcd.print(" on     sensor ");
  delay(1000);
  lcd.clear();
}

void found_id_place_vote()
{
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("found the Id  ");
  lcd.print(flag);
  lcd.setCursor(0, 1);
  lcd.print(" Place  Vote ");

  delay(1000);
  lcd.clear();
}

void place_vote()
{
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place     Vote");
  //lcd.setCursor(0, 1);
  //lcd.print("Below");
  delay(1000);
  lcd.clear();
}

void switch_off()
{
  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press     Switch");
  lcd.setCursor(0, 1);
  lcd.print("OFF");
  delay(1000);
  lcd.clear();
}

void reset_system()
{
  if (digitalRead (Reset) == 0) {
    start = 0;
    digitalWrite(buz, HIGH);

    if (timer > 0) {
      timer = timer - 1;
    }
    if (timer == 1) {
      for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.update(i, 0);
      }
    }

    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print("A=");
    lcd.print(EEPROM.read(1));
    //lcd.setCursor(0,1);
    lcd.print("    B=");
    lcd.print(EEPROM.read(2));
    lcd.setCursor(0, 1);
    lcd.print("C=");
    lcd.print(EEPROM.read(3));
    //lcd.setCursor(0,3);
    lcd.print("    D=");
    lcd.print(EEPROM.read(4));

    delay(2000);
    lcd.clear();

    //setup();
  } else {
    digitalWrite(buz, LOW);
    timer = 2;
  }
}
