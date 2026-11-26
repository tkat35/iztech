// 5. MESAFE + MOTOR: elimi takip et
// Robot onundeki ele ya da kutuya 15 cm uzaklikta durmaya calisir:
// uzaklasirsan gelir, yaklasirsan geri kacar.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;

const int HIZ = 90;
const int HEDEF_CM = 15;
const int TOLERANS = 3;     // 12-18 cm arasi "tamam" sayilir
const int MENZIL_CM = 60;   // Bundan uzaksa kimse yok, bekle

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
  sur(0, 0);
  delay(2000);
}

void loop() {
  long cm = olc();

  if (cm == 0 || cm > MENZIL_CM)        sur(0, 0);       // Kimse yok: bekle
  else if (cm > HEDEF_CM + TOLERANS)    sur(HIZ, HIZ);   // Uzak: yaklas
  else if (cm < HEDEF_CM - TOLERANS)    sur(-HIZ, -HIZ); // Yakin: geri kac
  else                                  sur(0, 0);       // Tam yerinde: dur

  delay(30);
}
