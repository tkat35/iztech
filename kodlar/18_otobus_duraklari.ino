// 18. SENARYO: OTOBUS DURAKLARI
// Cizgide git; her durakta (genis siyah bant) dur, servoyla "kapiyi" ac-kapat,
// sonra devam et. Son durakta tamamen dur.

#include <Servo.h>

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;
const byte SERVO_PIN = 13;

const int HIZ = 85, KP = 25;
const byte DURAK_SAYISI = 3;
const int BEKLEME_MS = 2000;
const int KAPI_KAPALI = 90, KAPI_ACIK = 150;

const int AGIRLIK[3] = {-2, 0, 2};
bool s[3];
int sonHata = 0;
byte durakNo = 0;
byte bantSayaci = 0;

Servo kapi;

void motor(byte pwm, byte in1, byte in2, int hiz) {
  hiz = constrain(hiz, -255, 255);
  digitalWrite(in1, hiz >= 0 ? HIGH : LOW);
  digitalWrite(in2, hiz >= 0 ? LOW : HIGH);
  analogWrite(pwm, abs(hiz));
}
void sur(int sol, int sag) {
  motor(SOL_PWM, SOL_IN1, SOL_IN2, sol);
  motor(SAG_PWM, SAG_IN1, SAG_IN2, sag);
}

bool cizgiVar(byte pin) {
  int d = digitalRead(pin);
  return SIYAH_LOW ? d == LOW : d == HIGH;
}

void pTakip(int temel) {
  int toplam = 0, goren = 0;
  for (byte i = 0; i < 3; i++) if (s[i]) { toplam += AGIRLIK[i]; goren++; }
  int hata;
  if (goren > 0) { hata = toplam / goren; sonHata = hata; }
  else hata = sonHata < 0 ? -3 : (sonHata > 0 ? 3 : 0);
  int d = KP * hata;
  sur(temel + d, temel - d);
}

void durakta() {
  sur(0, 0);
  kapi.write(KAPI_ACIK);  delay(BEKLEME_MS);   // Yolcu iniyor-biniyor
  kapi.write(KAPI_KAPALI); delay(500);
  durakNo++;
  if (durakNo < DURAK_SAYISI) {
    sur(HIZ, HIZ); delay(300);                 // Bandi gec, takibe don
  }
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
  kapi.attach(SERVO_PIN);
  kapi.write(KAPI_KAPALI);
  sur(0, 0);
  delay(3000);
}

void loop() {
  if (durakNo >= DURAK_SAYISI) { sur(0, 0); return; }   // Hat bitti

  for (byte i = 0; i < 3; i++) s[i] = cizgiVar(CIZGI[i]);
  bool bant = s[0] && s[1] && s[2];
  bantSayaci = bant ? bantSayaci + 1 : 0;

  if (bantSayaci >= 3) { bantSayaci = 0; durakta(); return; }
  pTakip(HIZ);
  delay(5);
}
