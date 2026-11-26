// 16. SENARYO: KESIK CIZGI
// Cizgi kesilince once duz devam et (bosluk olabilir), bulamazsan son yone ara,
// yine bulamazsan guvenli dur.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;

const int HIZ = 85, KP = 25, ARAMA_HIZI = 65;
const unsigned long BOSLUK_MS = 500;   // Bu kadar sure duz git (bosluk uzunluguna gore)
const unsigned long ARAMA_MS = 1200;   // Sonra bu kadar sure don ve ara

const int AGIRLIK[3] = {-2, 0, 2};
bool s[3];
int sonHata = 0;
unsigned long kayipBaslangic = 0;      // 0 = cizgi kayip degil

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
  delay(3000);
}

void loop() {
  int toplam = 0, goren = 0;
  for (byte i = 0; i < 3; i++) {
    s[i] = cizgiVar(CIZGI[i]);
    if (s[i]) { toplam += AGIRLIK[i]; goren++; }
  }

  if (goren > 0) {                       // Cizgi var: P ile izle
    kayipBaslangic = 0;
    sonHata = toplam / goren;
    int d = KP * sonHata;
    sur(HIZ + d, HIZ - d);
    delay(5);
    return;
  }

  // Cizgi kayip
  if (kayipBaslangic == 0) kayipBaslangic = millis();
  unsigned long gecen = millis() - kayipBaslangic;

  if (gecen < BOSLUK_MS) {
    sur(HIZ, HIZ);                       // 1) Boslugu duz gec
  } else if (gecen < BOSLUK_MS + ARAMA_MS) {
    int yon = sonHata < 0 ? -1 : 1;      // 2) Son gorulen tarafa don, ara
    sur(yon * ARAMA_HIZI, -yon * ARAMA_HIZI);
  } else {
    sur(0, 0);                           // 3) Bulamadi: guvenli dur
  }
  delay(5);
}
