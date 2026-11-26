// 20. SENARYO: ALANI TEMIZLE
// Siyah bantla cevrili alanda yerinde donerek nesne arar, bulunca iter.
// Kenara gelince (nesne disari cikmistir) geri cekilip yeniden arar.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;

const int ITME_HIZI = 110;
const int ARAMA_HIZI = 70;
const int GORUS_CM = 40;      // Alan buyuklugune gore: alan disini gormemeli
const int DONUS_HIZI = 100;   // 1. uygulamadaki deger
const int DONUS_MS = 450;     // 90 derece (1. uygulama)

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
  unsigned long sure = pulseIn(ECHO, HIGH, 6000);   // En fazla ~1 m
  return sure == 0 ? 0 : sure / 58;
}

bool cizgiVar(byte pin) {
  int d = digitalRead(pin);
  return SIYAH_LOW ? d == LOW : d == HIGH;
}
bool kenarda() {
  for (byte i = 0; i < 3; i++) if (cizgiVar(CIZGI[i])) return true;
  return false;
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  delay(3000);
}

void loop() {
  if (kenarda()) {                          // Kenar: geri cekil, arkani don
    sur(-ITME_HIZI, -ITME_HIZI); delay(500);
    sur(DONUS_HIZI, -DONUS_HIZI); delay(DONUS_MS * 2);
    sur(0, 0); delay(100);
    return;
  }

  long cm = olc();
  if (cm > 0 && cm < GORUS_CM) sur(ITME_HIZI, ITME_HIZI);   // Nesne onde: it
  else                         sur(ARAMA_HIZI, -ARAMA_HIZI); // Yerinde donerek ara
  delay(20);
}
