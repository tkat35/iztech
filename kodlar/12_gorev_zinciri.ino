// 12. GOREV ZINCIRI: cizgide git, hedef bantta dur, yuku birak, geri cekil
// Acilista kiskac acilir; 3 saniye icinde yuku kiskaca koyun, kiskac kapanir.
// Hedef: cizginin sonundaki genis siyah bant.

#include <Servo.h>

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;        // 2. uygulamadaki deger
const byte SERVO_PIN = 13;

const int HIZ = 80, YAVAS = 40, ARA = 70;   // 7. uygulamadaki degerler
const int ACIK = 110, KAPALI = 55;          // 4. uygulamadaki acilar
const byte BANT_OKUMA = 5;          // Bandi kac kez ust uste gorunce durulsun
const unsigned long ARAMA_MS = 1500;

enum Adim { GIT, BIRAK, BITTI };
Adim adim = GIT;
byte bantSayaci = 0;
int sonYon = 0;
unsigned long kayipBaslangic = 0;
bool s[3];

Servo kiskac;

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

void cizgiIzle() {
  if (s[0] || s[1] || s[2]) kayipBaslangic = 0;
  if (s[1] && s[0] == s[2]) { sur(HIZ, HIZ);       sonYon = 0;  }  // Ortada: duz
  else if (s[0] && s[1])    { sur(YAVAS, HIZ);     sonYon = -1; }  // Biraz solda
  else if (s[0])            { sur(YAVAS / 2, HIZ); sonYon = -1; }  // Cok solda: sert sola
  else if (s[2] && s[1])    { sur(HIZ, YAVAS);     sonYon = 1;  }  // Biraz sagda
  else if (s[2])            { sur(HIZ, YAVAS / 2); sonYon = 1;  }  // Cok sagda: sert saga
  else {
    if (kayipBaslangic == 0) kayipBaslangic = millis();
    if (millis() - kayipBaslangic > ARAMA_MS) sur(0, 0);
    else if (sonYon < 0) sur(-ARA, ARA);
    else if (sonYon > 0) sur(ARA, -ARA);
    else                 sur(0, 0);
  }
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  kiskac.attach(SERVO_PIN);
  kiskac.write(ACIK);               // Yuku koymak icin ac
  delay(3000);
  kiskac.write(KAPALI);             // Yuku tut
  delay(1000);
}

void loop() {
  for (byte i = 0; i < 3; i++) s[i] = cizgiVar(CIZGI[i]);

  switch (adim) {
    case GIT: {
      bool bant = s[0] && s[1] && s[2];
      bantSayaci = bant ? bantSayaci + 1 : 0;
      if (bantSayaci >= BANT_OKUMA) { sur(0, 0); adim = BIRAK; }
      else cizgiIzle();
      break;
    }
    case BIRAK:
      delay(300);
      kiskac.write(ACIK);           // Yuku birak
      delay(600);
      sur(-HIZ, -HIZ);
      delay(500);                   // Yukten uzaklas
      sur(0, 0);
      adim = BITTI;
      break;

    case BITTI:
      sur(0, 0);
      break;
  }
  delay(5);
}
