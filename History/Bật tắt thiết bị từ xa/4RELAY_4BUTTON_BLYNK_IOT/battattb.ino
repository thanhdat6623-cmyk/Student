#define BLYNK_TEMPLATE_ID "TMPL63fEIWvL9"
#define BLYNK_TEMPLATE_NAME "NCKHSV"
#include <RCSwitch.h>
#define DEBUG
#include "espConfig.h"
BlynkTimer timer;
int timerID1;
RCSwitch mySwitch = RCSwitch();
const int dataPin = 4;
const int led = 12;
const int button = 19;

boolean buttonState = HIGH;
unsigned long timesPush = millis();
unsigned long timesBlink = millis();
unsigned long codeRF1[4] = {};
unsigned long codeTemp;
int n = 0;
int modeRF = 0;
WidgetLED LED_KETNOI(V0);
#define BT1 V1
#define BT2 V2
#define BT3 V3
#define BT4 V4
unsigned long timeT = millis();
unsigned long timeDelay = millis();
const int out1 = 23;
const int out2 = 22;
const int out3 = 1;
const int out4 = 3;
const int btn1 = 34;
const int btn2 = 35;
const int btn3 = 32;
const int btn4 = 33;
boolean updateState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);
  attachInterrupt(btn1, handleButton, FALLING);
  attachInterrupt(btn2, handleButton, FALLING);
  attachInterrupt(btn3, handleButton, FALLING);
  attachInterrupt(btn4, handleButton, FALLING);

  espConfig.begin();

  Serial.println("\n========Read EEPROM============");
  for (int n = 0; n < 4; ++n) {
    codeRF1[n] = readLong(0 + (n * 4));
  }
  Serial.println("-----------CodeRF1---------------");
  Serial.println("1: " + String(codeRF1[0]));
  Serial.println("2: " + String(codeRF1[1]));
  Serial.println("3: " + String(codeRF1[2]));
  Serial.println("4: " + String(codeRF1[3]));
  Serial.println("-------------------------------");
  mySwitch.enableReceive(dataPin);
  Serial.println("-----------------Init------------------");
}

void loop() {
  espConfig.run();
  if (millis() - timeT > 1000) {
    if (LED_KETNOI.getValue()) LED_KETNOI.off();
    else LED_KETNOI.on();
    timeT = millis();
  }
  if (updateState == 1) {
    updateBlynk();
    updateState = 0;
  }
  app_loop();
}
void app_loop() {
  timer.run();
  checkButton();
  if (modeRF == 1) {
    if (mySwitch.available()) {
      if (codeTemp != mySwitch.getReceivedValue()) {
        Serial.print("Received ");
        Serial.print(mySwitch.getReceivedValue());
        Serial.print(" / ");
        Serial.print(mySwitch.getReceivedBitlength());
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.println(mySwitch.getReceivedProtocol());
        codeTemp = mySwitch.getReceivedValue();
        mySwitch.resetAvailable();
        if (n == 0) {
          codeRF1[0] = codeTemp;
          Serial.println("CodeRF1[0]: " + String(codeRF1[0]));
          n = 1;
          for (int i = 0; i < 10; ++i) {
            digitalWrite(led, !digitalRead(led));
            delay(100);
          }
          timer.restartTimer(timerID1);
        } else if (n == 1) {
          codeRF1[1] = codeTemp;
          Serial.println("CodeRF1[1]: " + String(codeRF1[1]));
          n = 2;
          for (int i = 0; i < 10; ++i) {
            digitalWrite(led, !digitalRead(led));
            delay(100);
          }
          timer.restartTimer(timerID1);
        } else if (n == 2) {
          codeRF1[2] = codeTemp;
          Serial.println("CodeRF1[2]: " + String(codeRF1[2]));
          n = 3;
          for (int i = 0; i < 10; ++i) {
            digitalWrite(led, !digitalRead(led));
            delay(100);
          }
          timer.restartTimer(timerID1);
        } else if (n == 3) {
          codeRF1[3] = codeTemp;
          Serial.println("CodeRF1[3]: " + String(codeRF1[3]));
          for (int n = 0; n < 4; ++n) {
            writeLong((0 + (n * 4)), codeRF1[n]);
          }
          EEPROM.commit();
          Serial.println("Đã lưu mã remote!");
          for (int i = 0; i < 20; ++i) {
            digitalWrite(led, !digitalRead(led));
            delay(100);
          }
          modeRF = 0;
          n = 0;
          digitalWrite(led, LOW);
          if (timer.isEnabled(timerID1)) {
            timer.deleteTimer(timerID1);
          }
        }
      }
    }
  } else {
    if (mySwitch.available()) {
      Serial.print("Received ");
      Serial.print(mySwitch.getReceivedValue());
      Serial.print(" / ");
      Serial.print(mySwitch.getReceivedBitlength());
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println(mySwitch.getReceivedProtocol());
      codeTemp = mySwitch.getReceivedValue();
      mySwitch.resetAvailable();
      if (millis() - timeDelay > 500) {
        if (codeTemp == codeRF1[0]) {
          Serial.println("CodeRF1!");
          digitalWrite(out1, !digitalRead(out1));
          updateState = 1;
        } else if (codeTemp == codeRF1[1]) {
          Serial.println("CodeRF2!");
          digitalWrite(out2, !digitalRead(out2));
          updateState = 1;
        } else if (codeTemp == codeRF1[2]) {
          Serial.println("CodeRF3!");
          digitalWrite(out3, !digitalRead(out3));
          updateState = 1;
        } else if (codeTemp == codeRF1[3]) {
          Serial.println("CodeRF4!");
          digitalWrite(out4, !digitalRead(out4));
          updateState = 1;
        }
        if (codeTemp == codeRF1[0] || codeTemp == codeRF1[1] || codeTemp == codeRF1[2] || codeTemp == codeRF1[3]) {
          digitalWrite(led, HIGH);
          delay(200);
          digitalWrite(led, LOW);
        }
        timeDelay = millis();
      }
    }
  }
}
ICACHE_RAM_ATTR void handleButton() {
  if (millis() - timeDelay > 500) {
    if (digitalRead(btn1) == LOW) digitalWrite(out1, !digitalRead(out1));
    if (digitalRead(btn2) == LOW) digitalWrite(out2, !digitalRead(out2));
    if (digitalRead(btn3) == LOW) digitalWrite(out3, !digitalRead(out3));
    if (digitalRead(btn4) == LOW) digitalWrite(out4, !digitalRead(out4));
    timeDelay = millis();
  }
}
BLYNK_CONNECTED() {
  Blynk.syncAll();  //Đồng bộ data từ server xuống esp khi kết nối
}
BLYNK_WRITE(BT1) {
  int p = param.asInt();
  digitalWrite(out1, p);
}
BLYNK_WRITE(BT2) {
  int p = param.asInt();
  digitalWrite(out2, p);
}
BLYNK_WRITE(BT3) {
  int p = param.asInt();
  digitalWrite(out3, p);
}
BLYNK_WRITE(BT4) {
  int p = param.asInt();
  digitalWrite(out4, p);
}
void updateBlynk() {
  Blynk.virtualWrite(BT1, digitalRead(out1));
  Blynk.virtualWrite(BT2, digitalRead(out2));
  Blynk.virtualWrite(BT3, digitalRead(out3));
  Blynk.virtualWrite(BT4, digitalRead(out4));
}
void handleTimerID1() {
  modeRF = 0;
  digitalWrite(led, LOW);
  Serial.println("\n=====Thoát chế độ học mã========");
  for (int n = 0; n < 4; ++n) {
    codeRF1[n] = readLong(0 + (n * 4));
  }
  Serial.println("-----------CodeRF1---------------");
  Serial.println("1: " + String(codeRF1[0]));
  Serial.println("2: " + String(codeRF1[1]));
  Serial.println("3: " + String(codeRF1[2]));
  Serial.println("4: " + String(codeRF1[3]));
  Serial.println("-------------------------------");
}
void writeLong(int address, unsigned long num) {
  EEPROM.write(address, (num >> 24) & 0xFF);
  EEPROM.write(address + 1, (num >> 16) & 0xFF);
  EEPROM.write(address + 2, (num >> 8) & 0xFF);
  EEPROM.write(address + 3, num & 0xFF);
}
unsigned long readLong(int address) {
  return ((unsigned long)EEPROM.read(address) << 24) + ((unsigned long)EEPROM.read(address + 1) << 16) + ((unsigned long)EEPROM.read(address + 2) << 8) + (unsigned long)EEPROM.read(address + 3);
}
void checkButton() {
  if (digitalRead(button) == LOW) {
    if (buttonState == HIGH) {
      buttonState = LOW;
      modeRF = 0;
      codeTemp = 0;
      mySwitch.resetAvailable();
      n = 0;
      digitalWrite(led, LOW);
      if (timer.isEnabled(timerID1)) timer.deleteTimer(timerID1);
      Serial.println("Ấn giữ 3 giây để vào chế độ học mã remote1/2!");
      delay(500);
    }
    if (millis() - timesPush > 3000) {
      if (modeRF == 0) {
        modeRF = 1;
        Serial.println("Chế độ học mã remote!");
        digitalWrite(led, HIGH);
        timerID1 = timer.setTimeout(15000, handleTimerID1);  //thoát học lệnh sau 15s
      }
    }
    if (millis() - timesPush > 10000) {
      if (modeRF == 1) {
        for (int i = 0; i < 16; ++i) {
          EEPROM.write((i), 0);
        }
        EEPROM.commit();
        modeRF = 2;
        Serial.println("Đã xóa mã remote!");
        if (timer.isEnabled(timerID1)) timer.deleteTimer(timerID1);
        for (int n = 0; n < 4; ++n) {
          codeRF1[n] = 0;
        }
      }
    }
  } else {
    timesPush = millis();
    buttonState = HIGH;
  }
}
