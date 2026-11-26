// 12. GOREV ZINCIRI: cizgide git, hedef bantta dur, yuku birak, geri cekil
// Basta kiskac kapali ve nesneyi tutuyor. Hedef: cizginin sonundaki genis siyah bant.

#include <Servo.h>

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[4] = {A0, A1, A2, A3};
const bool SIYAH_LOW = true;
const byte SERVO_PIN = 3;

const int HIZ = 80, YAVAS = 35, ARA = 55;
const int ACIK = 110, KAPALI = 55;
const byte BANT_OKUMA = 5;          // Bandi kac kez ust uste gorunce durulsun

enum Adim { GIT, BIRAK, BITTI };
Adim adim = GIT;
byte bantSayaci = 0;
int sonYon = 0;
bool s[4];

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
  if (s[1] && s[2])  { sur(HIZ, HIZ);       sonYon = 0;  }
  else if (s[0])     { sur(YAVAS / 2, HIZ); sonYon = -1; }
  else if (s[1])     { sur(YAVAS, HIZ);     sonYon = -1; }
  else if (s[3])     { sur(HIZ, YAVAS / 2); sonYon = 1;  }
  else if (s[2])     { sur(HIZ, YAVAS);     sonYon = 1;  }
  else if (sonYon < 0) sur(-ARA, ARA);
  else if (sonYon > 0) sur(ARA, -ARA);
  else                 sur(0, 0);
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 4; i++) pinMode(CIZGI[i], INPUT);
  kiskac.attach(SERVO_PIN);
  kiskac.write(KAPALI);             // Yuk elde
  sur(0, 0);
  delay(2000);
}

void loop() {
  for (byte i = 0; i < 4; i++) s[i] = cizgiVar(CIZGI[i]);

  switch (adim) {
    case GIT: {
      bool bant = s[0] && s[1] && s[2] && s[3];
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
