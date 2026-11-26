// 17. SENARYO: ENGELI DOLAS, CIZGIYE DON
// Cizgiyi izle. Onde engel gorunce sagdan dolas, cizgiyi bulup yoluna devam et.
// Mesafe sensoru onde sabit. Sureler (ms) robotunuza gore ayarlanir.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;

const int HIZ = 85, KP = 25, DONUS_HIZI = 100;
const int ENGEL_CM = 12;
const int DONUS_MS = 450;     // 90 derece (1. uygulama)
const int YAN_MS = 600;       // Engelin yanina cikma suresi
const int GEC_MS = 900;       // Engelin yanindan gecme suresi

const int AGIRLIK[3] = {-2, 0, 2};
bool s[3];
int sonHata = 0;

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
void sensorOku() {
  for (byte i = 0; i < 3; i++) s[i] = cizgiVar(CIZGI[i]);
}
bool herhangiCizgi() { return s[0] || s[1] || s[2]; }

void pTakip(int temel) {
  int toplam = 0, goren = 0;
  for (byte i = 0; i < 3; i++) if (s[i]) { toplam += AGIRLIK[i]; goren++; }
  int hata;
  if (goren > 0) { hata = toplam / goren; sonHata = hata; }
  else hata = sonHata < 0 ? -3 : (sonHata > 0 ? 3 : 0);
  int d = KP * hata;
  sur(temel + d, temel - d);
}

void adim(int sol, int sag, int ms) { sur(sol, sag); delay(ms); sur(0, 0); delay(100); }

void dolas() {
  sur(0, 0); delay(200);
  adim(DONUS_HIZI, -DONUS_HIZI, DONUS_MS);   // Saga don
  adim(HIZ, HIZ, YAN_MS);                    // Yana cik
  adim(-DONUS_HIZI, DONUS_HIZI, DONUS_MS);   // Sola don (yine ileri bakiyor)
  adim(HIZ, HIZ, GEC_MS);                    // Engelin yanindan gec
  adim(-DONUS_HIZI, DONUS_HIZI, DONUS_MS);   // Sola don (cizgiye bakiyor)

  unsigned long baslangic = millis();        // Cizgiyi bulana kadar ileri
  do { sensorOku(); sur(HIZ, HIZ); } while (!herhangiCizgi() && millis() - baslangic < 2500);
  sur(HIZ, HIZ); delay(150);                 // Tekerleri cizginin ustune getir

  sur(DONUS_HIZI, -DONUS_HIZI); delay(200);  // Saga donerek cizgiye hizalan
  baslangic = millis();
  do { sensorOku(); } while (!s[1] && millis() - baslangic < 2000);
  sur(0, 0);
  sonHata = 0;
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
  long cm = olc();
  if (cm > 0 && cm < ENGEL_CM) { dolas(); return; }
  sensorOku();
  pTakip(HIZ);
}
