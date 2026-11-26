// 14. CIZGI IZLEME: PD KONTROL
// P: hata ne kadar buyuk?   D: hata ne kadar hizli degisiyor?
// D, robotun cizginin iki yanina savrulmasini (zikzak) azaltir.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;

const int TEMEL_HIZ = 120;
const int KP = 22;           // 13. uygulamada buldugunuz degerden biraz kucuk baslayin
const int KD = 15;           // 0'dan baslayip artirin; zikzak azalinca durun

const int AGIRLIK[3] = {-2, 0, 2};
int sonHata = 0;             // Kayip durumunda yon icin
int oncekiHata = 0;          // D icin bir onceki dongudeki hata
const unsigned long ARAMA_MS = 1500;   // Cizgi bu kadar sure gorulmezse dur
unsigned long sonGorulme = 0;

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

int hataOku() {
  int toplam = 0, goren = 0;
  for (byte i = 0; i < 3; i++) {
    if (cizgiVar(CIZGI[i])) { toplam += AGIRLIK[i]; goren++; }
  }
  if (goren == 0) {
    return sonHata < 0 ? -3 : (sonHata > 0 ? 3 : 0);
  }
  sonGorulme = millis();
  sonHata = toplam / goren;
  return sonHata;
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  delay(3000);
  sonGorulme = millis();
}

void loop() {
  int hata = hataOku();
  if (millis() - sonGorulme > ARAMA_MS) { sur(0, 0); return; }   // Cizgi bulunamadi: dur
  int degisim = hata - oncekiHata;          // D: hata ne kadar degisti?
  oncekiHata = hata;

  int duzeltme = KP * hata + KD * degisim;  // PD = Kp x hata + Kd x degisim
  sur(TEMEL_HIZ + duzeltme, TEMEL_HIZ - duzeltme);
  delay(5);                                 // Her dongu ayni surede olsun; D buna baglidir
}
