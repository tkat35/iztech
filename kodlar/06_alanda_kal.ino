// 6. CIZGI + MOTOR: siyah bantla cevrili alanin icinde kal
// Beyaz zemine siyah bantla bir alan cizin. Robot ileri gider,
// kenari gorunce geri cekilip iceri doner.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;

const int HIZ = 90;
const int GERI_MS = 400;
const int DONUS_HIZI = 100; // 1. uygulamadaki deger
const int DONUS_MS = 450;   // 1. uygulamada buldugunuz 90 derece suresi

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

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  randomSeed(analogRead(A5));   // "Bir adim ote" icin: random() her acilista farkli olsun
  delay(3000);
}

void loop() {
  bool solKenar = cizgiVar(CIZGI[0]);
  bool sagKenar = cizgiVar(CIZGI[2]) || cizgiVar(CIZGI[1]);

  if (!solKenar && !sagKenar) {
    sur(HIZ, HIZ);              // Alan icinde: ileri
    return;
  }

  sur(0, 0);       delay(100);
  sur(-HIZ, -HIZ); delay(GERI_MS);           // Kenardan geri cekil
  if (solKenar) sur(DONUS_HIZI, -DONUS_HIZI);  // Kenar soldaysa saga don
  else          sur(-DONUS_HIZI, DONUS_HIZI);  // Kenar sagdaysa sola don
  delay(DONUS_MS);
  sur(0, 0);       delay(100);
}
