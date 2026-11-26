// 9. MOTOR + SERVO + MESAFE: engelden kacan robot
// Ileri gider; onde engel varsa durur, saga ve sola bakar, acik tarafa doner.
// Sensor servonun ustunde (8. uygulamadaki gibi).

#include <Servo.h>

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;
const byte SERVO_PIN = 3;

const int HIZ = 90;
const int ENGEL_CM = 20;
const int DONUS_HIZI = 100;         // 1. uygulamadaki deger
const int DONUS_MS = 450;           // 1. uygulamadaki 90 derece suresi
const int SOL_ACI = 150, SAG_ACI = 30;   // 8. uygulamada hangi aci sola bakiyorsa onu yazin

Servo kafa;
long sonGecerli = 400;              // En son alinan gecerli (0 olmayan) olcum

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

long bak(int aci) {                 // Servoyu cevir, olc
  kafa.write(aci);
  delay(450);                       // Servo yerine otursun
  long cm = olc();
  return cm == 0 ? 400 : cm;        // Yanlarda olcum yoksa "acik" kabul et
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  kafa.attach(SERVO_PIN);
  kafa.write(90);
  sur(0, 0);
  delay(3000);
}

void loop() {
  long on = olc();
  if (on > 0) {
    sonGecerli = on;
  } else if (sonGecerli <= 40) {
    on = 1;                         // Az once yakinda bir sey vardi ve yanki kayboldu: engel say
  } else {
    on = 400;                       // Onceden de uzaktaydi: yol acik
  }

  if (on > ENGEL_CM) {              // Yol acik
    sur(HIZ, HIZ);
    delay(30);
    return;
  }

  sur(0, 0);       delay(150);
  sur(-HIZ, -HIZ); delay(250);      // Biraz geri
  sur(0, 0);

  long sol = bak(SOL_ACI);
  long sag = bak(SAG_ACI);
  kafa.write(90);
  sonGecerli = 400;

  if (sol >= sag) sur(-DONUS_HIZI, DONUS_HIZI);   // Sol daha acik: sola don
  else            sur(DONUS_HIZI, -DONUS_HIZI);   // Sag daha acik: saga don
  delay(DONUS_MS);
  sur(0, 0);
  delay(300);
}
