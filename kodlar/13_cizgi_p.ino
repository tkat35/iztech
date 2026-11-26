// 13. CIZGI IZLEME: P KONTROL
// Cizgi ortadan ne kadar uzaksa o kadar cok duzeltir.
// Hata: en sol -3, ic sol -1, ic sag +1, en sag +3 (goren gozlerin ortalamasi).

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[4] = {A0, A1, A2, A3};
const bool SIYAH_LOW = true;

const int TEMEL_HIZ = 100;   // Cizgi tam ortadayken iki tekerin hizi
const int KP = 25;           // Hata basina duzeltme miktari. Once bunu ayarlayin.

const int AGIRLIK[4] = {-3, -1, 1, 3};
const unsigned long ARAMA_MS = 1500;   // Cizgi bu kadar sure gorulmezse dur
int sonHata = 0;
unsigned long sonGorulme = 0;          // Cizginin en son goruldugu an

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

// Cizginin ortadan uzakligi: -3 (cok solda) ... 0 (ortada) ... +3 (cok sagda)
int hataOku() {
  int toplam = 0, goren = 0;
  for (byte i = 0; i < 4; i++) {
    if (cizgiVar(CIZGI[i])) { toplam += AGIRLIK[i]; goren++; }
  }
  if (goren == 0) {                 // Cizgi kayip: son gorulen tarafa en sert donus
    return sonHata < 0 ? -4 : (sonHata > 0 ? 4 : 0);
  }
  sonGorulme = millis();
  sonHata = toplam / goren;
  return sonHata;
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 4; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  delay(3000);
  sonGorulme = millis();
}

void loop() {
  int hata = hataOku();
  if (millis() - sonGorulme > ARAMA_MS) { sur(0, 0); return; }   // Cizgi bulunamadi: dur
  int duzeltme = KP * hata;                       // P = Kp x hata
  sur(TEMEL_HIZ + duzeltme, TEMEL_HIZ - duzeltme); // Cizgi soldaysa sol teker yavaslar
  delay(5);
}
