#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLMfqtUY3m"
#define BLYNK_TEMPLATE_NAME "HMK eFARM BUDIDAYA JAMUR Lapas Sukamiskin"
#define BLYNK_AUTH_TOKEN "lTKt0N6AXqe_r2rn0mm4Um_9pM40_2rJ"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Budidaya_Jamur";
char pass[] = "zz11xx22cB";
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

String textMode;

// KONFIGURASI RELAY
#define relayPompa D5
#define relaySprinkle D6
int pinValue0 = 0;
int pinValue1 = 0;
int pinValue2 = 0;

float pinValue6 = 0;  // INPUT LINE A
int pinValue8 = 0;    // TOMBOL KIRIM LINE A
float pinValue22;     // TOTAL HASIL BULANAN LINE A
float hasilTotalA;

float pinValue11 = 0;  // INPUT LINE B
int pinValue12 = 0;    // TOMBOL KIRIM LINE B
float pinValue23;      // TOTAL HASIL BULANAN LINE B
float hasilTotalB;

int pinValue14 = 27;  // BATAS ATAS TEMP
int pinValue15 = 83;  // BATAS ATAS HUMD
int pinValue18 = 22;  // BATAS BAWAH TEMP
int pinValue19 = 78;  // BATAS BAWAH HUMD
int statusRelayPompa = 0;
int statusRelaySprinkle = 0;

// KONFIGURASI AHT20
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
float t;
float h;

//MODE
bool mode = true;

unsigned long previousMillis = 0;
const long interval = 1500;

//BUZZER
#define buzz 25
BlynkTimer timer;

BLYNK_WRITE(V0) {
  pinValue0 = param.asInt();
}
BLYNK_WRITE(V1) {
  pinValue1 = param.asInt();
}
BLYNK_WRITE(V2) {
  pinValue2 = param.asInt();
  // if (pinValue2 == 1) {
  //   Blynk.logEvent("mode_otomatis_hidup");
  // } else {
  //   Blynk.logEvent("mode_otomatis_mati");
  // }
}

BLYNK_WRITE(V6) {  // INPUT HASIL LINE A
  pinValue6 = param.asFloat();
}
BLYNK_WRITE(V8) {  // KIRIM HASIL LINE A
  pinValue8 = param.asInt();
}

BLYNK_WRITE(V11) {  // INPUT HASIL LINE B
  pinValue11 = param.asFloat();
}
BLYNK_WRITE(V12) {  // KIRIM HASILI LINE B
  pinValue12 = param.asInt();
}
BLYNK_WRITE(V14) {
  pinValue14 = param.asInt();
}
BLYNK_WRITE(V15) {
  pinValue15 = param.asInt();
}
BLYNK_WRITE(V18) {
  pinValue18 = param.asInt();
}
BLYNK_WRITE(V19) {
  pinValue19 = param.asInt();
}
BLYNK_WRITE(V22) {
  pinValue22 = param.asFloat();
}
BLYNK_WRITE(V23) {
  pinValue23 = param.asFloat();
}

void sendSensor() {
  if (statusRelayPompa == 1) {
    Blynk.virtualWrite(V0, 1);
  } else {
    Blynk.virtualWrite(V0, 0);
  }

  if (statusRelaySprinkle == 1) {
    Blynk.virtualWrite(V1, 1);
  } else {
    Blynk.virtualWrite(V1, 0);
  }

  Blynk.virtualWrite(V4, t);
  Blynk.virtualWrite(V5, h);

  if (pinValue8 == 1) {
    Blynk.virtualWrite(V9, pinValue6);
    hasilTotalA = pinValue6 + pinValue22;
    Blynk.virtualWrite(V22, hasilTotalA);
    Blynk.virtualWrite(V6, 0);
    Blynk.virtualWrite(V8, 0);
    delay(1000);
  }

  if (pinValue12 == 1) {
    Blynk.virtualWrite(V13, pinValue11);
    hasilTotalB = pinValue11 + pinValue23;
    Blynk.virtualWrite(V23, hasilTotalB);
    Blynk.virtualWrite(V11, 0);
    Blynk.virtualWrite(V12, 0);
    delay(1000);
  }

  if (mode == true) {
    Blynk.virtualWrite(V3, "BLOWER OTOMATIS");
    Blynk.virtualWrite(V2, 1);
  } else {
    Blynk.virtualWrite(V3, "BLOWER MANUAL");
    Blynk.virtualWrite(V2, 0);
  }
  // Serial.println("Data Sudah Terkirim");
}

void modeManual() {
  if (pinValue0 == 1) {
    digitalWrite(relayPompa, LOW);
    statusRelayPompa = 1;
  } else {
    digitalWrite(relayPompa, HIGH);
    statusRelayPompa = 0;
  }

  if (pinValue1 == 1) {
    digitalWrite(relaySprinkle, LOW);
    statusRelaySprinkle = 1;
  } else {
    digitalWrite(relaySprinkle, HIGH);
    statusRelaySprinkle = 0;
  }
}

void modeOtomatis() {
  // if (t >= pinValue14 && h <= pinValue19) {
  //   digitalWrite(relaySprinkle, LOW);
  //   statusRelaySprinkle = 1;
  // } else if (t >= pinValue18 && h >= pinValue15) {
  //   digitalWrite(relaySprinkle, HIGH);
  //   statusRelaySprinkle = 0;
  // }

  // if (h <= pinValue19) {
  //   digitalWrite(relaySprinkle, LOW);
  //   statusRelaySprinkle = 1;
  // } else if (h >= pinValue15) {
  //   digitalWrite(relaySprinkle, HIGH);
  //   statusRelaySprinkle = 0;
  // }

  if (h >= pinValue15) {
    digitalWrite(relayPompa, LOW);
    statusRelayPompa = 1;
  } else if (h <= pinValue19) {
    digitalWrite(relayPompa, HIGH);
    statusRelayPompa = 0;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPompa, OUTPUT);
  pinMode(relaySprinkle, OUTPUT);
  pinMode(buzz, OUTPUT);
  digitalWrite(relayPompa, HIGH);
  digitalWrite(relaySprinkle, HIGH);
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     SMART      ");
  lcd.setCursor(0, 1);
  lcd.print("    FARMING     ");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Powered By   ");
  lcd.setCursor(0, 1);
  lcd.print("  RUMAH INOVASI ");
  delay(1500);
  lcd.clear();
  if (!aht.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("     SENSOR   ");
    lcd.setCursor(0, 1);
    lcd.print(" NOT CONNECTED");
    delay(1500);
    // while (1) delay(10);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("     AHT20     ");
    lcd.setCursor(0, 1);
    lcd.print("   CONNECTED   ");
    delay(1500);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   CONNECTING   ");
  lcd.setCursor(0, 1);
  lcd.print(" TO INTERNET... ");
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, sendSensor);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    INTERNET   ");
  lcd.setCursor(0, 1);
  lcd.print("   CONNECTED   ");
  lcd.clear();
  delay(1500);
}

void loop() {
  unsigned long currentMillis = millis();
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  t = temp.temperature;
  h = humidity.relative_humidity;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEMP: " + String(t) + "c " + textMode);
    lcd.setCursor(0, 1);
    lcd.print("HUMD: " + String(h) + "%");
  }

  if (pinValue2 == 1) {
    mode = true;
    // Serial.println("MODE OTOMATIS");
    textMode = "AT";
    // digitalWrite(buzz, HIGH);
    // delay(1000);
    // digitalWrite(buzz, LOW);
  } else {
    mode = false;
    // Serial.println("MODE MANUAL");
    textMode = "MN";
    // digitalWrite(buzz, HIGH);
    // delay(1000);
    // digitalWrite(buzz, LOW);
  }

  if (pinValue1 == 1) {
    digitalWrite(relaySprinkle, LOW);
    statusRelaySprinkle = 1;
  } else {
    digitalWrite(relaySprinkle, HIGH);
    statusRelaySprinkle = 0;
  }
  
  switch (mode) {
    case false:
      modeManual();
      break;

    case true:
      modeOtomatis();
      break;
  }
  Blynk.syncVirtual(V22, V23);
  Blynk.run();
  timer.run();
}
