// 19. SENARYO: LABIRENT (SOL EL KURALI)
// Mesafe sensoru servonun ustunde. Her hucrede sola, one, saga bakar:
// sol aciksa sola, degilse duz, degilse saga, hepsi kapaliysa geri doner.

#include <Servo.h>

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;
const byte SERVO_PIN = 3;

const int HIZ = 90, DONUS_HIZI = 100;
const int ACIK_CM = 25;       // Bundan uzaksa o yon acik
const int DONUS_MS = 450;     // 90 derece (1. uygulama)
const int HUCRE_MS = 700;     // Bir hucre ilerleme suresi
const int SOL_ACI = 150, ON_ACI = 90, SAG_ACI = 30;

Servo kafa;

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

long bak(int aci) {
  kafa.write(aci);
  delay(350);
  long cm = olc();
  return cm == 0 ? 400 : cm;  // Olcum yok = acik kabul
}

void solaDon()  { sur(-DONUS_HIZI, DONUS_HIZI); delay(DONUS_MS); sur(0, 0); delay(150); }
void sagaDon()  { sur(DONUS_HIZI, -DONUS_HIZI); delay(DONUS_MS); sur(0, 0); delay(150); }
void hucreIlerle() {
  sur(HIZ, HIZ);
  unsigned long baslangic = millis();
  while (millis() - baslangic < (unsigned long)HUCRE_MS) {
    long on = olc();
    if (on > 0 && on < 8) break;   // Duvara cok yaklastiysa erken dur
  }
  sur(0, 0);
  delay(150);
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  kafa.attach(SERVO_PIN);
  kafa.write(ON_ACI);
  sur(0, 0);
  delay(3000);
}

void loop() {
  long sol = bak(SOL_ACI);
  long on  = bak(ON_ACI);
  long sag = bak(SAG_ACI);
  kafa.write(ON_ACI);

  if (sol > ACIK_CM)      { solaDon(); hucreIlerle(); }
  else if (on > ACIK_CM)  { hucreIlerle(); }
  else if (sag > ACIK_CM) { sagaDon(); hucreIlerle(); }
  else                    { sagaDon(); sagaDon(); }     // Cikmaz: geri don
}
