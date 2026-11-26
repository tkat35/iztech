// 11. MESAFE + MOTOR + SERVO KISKAC: nesneye yaklas ve al
// Mesafe sensoru onde sabit; servo kiskaci acar/kapatir.

#include <Servo.h>

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;
const byte SERVO_PIN = 3;

const int YAKLASMA_HIZI = 70;
const int AL_CM = 8;                  // Nesne kiskacin icine girdiginde olculen mesafe
const int ACIK = 110, KAPALI = 55;    // 4. uygulamada buldugunuz acilar

enum Adim { YAKLAS, AL, BITTI };
Adim adim = YAKLAS;

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

long olc() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long sure = pulseIn(ECHO, HIGH, 25000);
  return sure == 0 ? 0 : sure / 58;
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  kiskac.attach(SERVO_PIN);
  kiskac.write(ACIK);
  sur(0, 0);
  delay(2000);
}

void loop() {
  switch (adim) {
    case YAKLAS: {
      long cm = olc();
      if (cm == 0)          sur(0, 0);                            // Olcum yok: bekle
      else if (cm > AL_CM)  sur(YAKLASMA_HIZI, YAKLASMA_HIZI);    // Yaklas
      else                  { sur(0, 0); adim = AL; }
      break;
    }
    case AL:
      delay(300);
      kiskac.write(KAPALI);           // Kavra
      delay(600);
      sur(-YAKLASMA_HIZI, -YAKLASMA_HIZI);
      delay(600);                     // Nesneyle geri cekil
      sur(0, 0);
      adim = BITTI;
      break;

    case BITTI:
      sur(0, 0);
      break;
  }
  delay(20);
}
