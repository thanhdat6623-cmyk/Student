#include "Config.h"
#include "mode_sepay.h"
#include "mode_bt.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Audio audio;
BluetoothA2DPSink a2dp_sink;
AsyncWebServer server(80);

bool buttonPressed = false;
bool longPress = false;
int currentVolume = 21;
unsigned long lastFetch = 0;
unsigned long pressStart = 0;
String lastTransactionID = "";
String lastTextToSpeak = "";
int currentMode = 0;

String WIFI_SSID = "";
String WIFI_PASSWORD = "";
String ACCOUNT_NUM = "";
String SEPAY_TOKEN = "";

void IRAM_ATTR handleButton() {
  if (!buttonPressed) {
    buttonPressed = true;
    pressStart = millis();
  }
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButton, FALLING);
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);
  if (!loadConfig()) {
    startConfigPortal();
    return;
  }
  if (currentMode == 0) startSepayMode();
  else startBluetoothMode();
}

void loop() {
  audio.loop();
  if (buttonPressed) {
    buttonPressed = false;
    unsigned long pressDuration = millis() - pressStart;
    if (pressDuration >= 5000) {
      startConfigPortal();
    } else if (pressDuration >= 3000) {
      currentMode = !currentMode;
      saveConfig();
      ESP.restart();
    } else if (currentMode == 0 && lastTextToSpeak.length() > 0) {
      audio.connecttospeech(lastTextToSpeak.c_str(), "vi");
    }
  }
  if (currentMode == 0 && millis() - lastFetch > 5000) {
    lastFetch = millis();
    fetchAndDisplay();
  }
}
