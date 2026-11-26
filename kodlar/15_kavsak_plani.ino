// 15. SENARYO: KAVSAK PLANI
// Cizgiyi P kontrolle izle. Her kavsakta PLAN listesindeki hareketi yap.
// Kavsak = dort goz birden siyah (cizgiye dik, sensorden genis bir bant).

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[4] = {A0, A1, A2, A3};
const bool SIYAH_LOW = true;

const int HIZ = 90, KP = 25, DONUS_HIZI = 80;
const int ORTALA_MS = 250;   // Kavsakta tekerleri kavsagin ortasina getirmek icin ileri sure

enum Hareket { DUZ, SOLA, SAGA, DUR };
const Hareket PLAN[] = { SAGA, SOLA, DUZ, DUR };   // Pistinize gore degistirin
const byte PLAN_UZUNLUK = sizeof(PLAN) / sizeof(PLAN[0]);

const int AGIRLIK[4] = {-3, -1, 1, 3};
bool s[4];
int sonHata = 0;
byte kavsakNo = 0;
byte bantSayaci = 0;
bool bitti = false;

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
void sensorOku() {
  for (byte i = 0; i < 4; i++) s[i] = cizgiVar(CIZGI[i]);
}

void pTakip(int temel) {
  int toplam = 0, goren = 0;
  for (byte i = 0; i < 4; i++) if (s[i]) { toplam += AGIRLIK[i]; goren++; }
  int hata;
  if (goren > 0) { hata = toplam / goren; sonHata = hata; }
  else hata = sonHata < 0 ? -4 : (sonHata > 0 ? 4 : 0);
  int d = KP * hata;
  sur(temel + d, temel - d);
}

// Yerinde don; orta gozler yeni cizgiyi bulunca dur
void cizgiyeDon(int yon) {                 // yon: -1 sol, +1 sag
  sur(yon * DONUS_HIZI, -yon * DONUS_HIZI);
  delay(200);                              // Once eski cizgiden ayril
  unsigned long baslangic = millis();
  do { sensorOku(); } while (!(s[1] || s[2]) && millis() - baslangic < 2000);
  sur(0, 0);
  sonHata = 0;
}

void kavsakta(Hareket h) {
  sur(HIZ, HIZ);
  delay(ORTALA_MS);
  if (h == DUR)       { sur(0, 0); bitti = true; }
  else if (h == SOLA) cizgiyeDon(-1);
  else if (h == SAGA) cizgiyeDon(1);
  // DUZ: zaten kavsagi gecti, takibe devam
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 4; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  delay(3000);
}

void loop() {
  if (bitti) { sur(0, 0); return; }

  sensorOku();
  bool kavsak = s[0] && s[1] && s[2] && s[3];
  bantSayaci = kavsak ? bantSayaci + 1 : 0;

  if (bantSayaci >= 3) {                   // 3 okuma ust uste: gercekten kavsak
    bantSayaci = 0;
    kavsakta(PLAN[kavsakNo]);
    kavsakNo++;
    if (kavsakNo >= PLAN_UZUNLUK) bitti = true;
    return;
  }
  pTakip(HIZ);
  delay(5);
}
