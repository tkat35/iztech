// TYY BASLANGIC TASLAGI - Odemis BILSEM
// Pinler ORNEKTIR. Kendi baglantiniza gore degistirin.
#include <Servo.h>

// ---- PINLER ----
const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;   // L298N sol motor
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;  // L298N sag motor
const byte TRIG = 11, ECHO = 12;                    // HC-SR04
const byte SERVO_PIN = 3;                           // Servo sinyal
const byte CIZGI[4] = {A0, A1, A2, A3};             // 4'lu sensor, soldan saga

// ---- AYARLAR (testte bulun) ----
const bool SIYAH_LOW = true;   // Siyahta sensor LOW veriyorsa true
const int  HIZ = 90;           // Temel hiz (0-255)
const int  DONUS_HIZI = 100;   // 1. uygulamadaki deger
const int  DONUS_MS = 450;     // 1. uygulamadaki 90 derece suresi
const int  KP = 25;            // P kontrol katsayisi (uygulama 13)
const int  YAKIN_CM = 10;      // Nesneye bu kadar yaklasinca dur
const int  SERVO_ACIK = 110, SERVO_KAPALI = 55;

Servo kol;

// ---- GOREV ADIMLARI: temaya gore burayi degistirin ----
enum Adim { BASLA, CIZGI_TAKIP, YAKLAS, AL, DONUS, BIRAK, BITTI };
Adim adim = BASLA;
unsigned long adimBaslangic = 0;

void adimaGec(Adim yeni) {
  adim = yeni;
  adimBaslangic = millis();
  Serial.print("Adim: ");
  Serial.println(yeni);
}
unsigned long adimSuresi() { return millis() - adimBaslangic; }

// ---- MOTOR ----
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
void dur() { sur(0, 0); }

// ---- MESAFE ----
long mesafeCm() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long sure = pulseIn(ECHO, HIGH, 6000);   // En fazla ~1 m
  return sure == 0 ? 0 : sure / 58;   // 0 = olcum yok
}

// ---- CIZGI ----
bool s[4];
int sonHata = 0;
bool cizgiVar(byte pin) {
  int d = digitalRead(pin);
  return SIYAH_LOW ? d == LOW : d == HIGH;
}
int sensorSay() {   // Kac goz cizgiyi goruyor? (0-4)
  int n = 0;
  for (byte i = 0; i < 4; i++) { s[i] = cizgiVar(CIZGI[i]); if (s[i]) n++; }
  return n;
}
void cizgiTakip(int hiz) {   // P kontrol, agirliklar -3 -1 +1 +3
  const int AGIRLIK[4] = {-3, -1, 1, 3};
  long toplam = 0; int n = 0;
  for (byte i = 0; i < 4; i++) if (s[i]) { toplam += AGIRLIK[i]; n++; }
  int hata = (n > 0) ? toplam / n : sonHata;
  sonHata = hata;
  int duzeltme = KP * hata;
  sur(hiz + duzeltme, hiz - duzeltme);
}

void setup() {
  Serial.begin(9600);
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  for (byte i = 0; i < 4; i++) pinMode(CIZGI[i], INPUT);
  kol.attach(SERVO_PIN);
  kol.write(SERVO_ACIK);
  dur();
  adimaGec(BASLA);
}

void loop() {
  int gorenSensor = sensorSay();

  switch (adim) {
    case BASLA:                       // 2 sn bekle, sonra yola cik
      if (adimSuresi() > 2000) adimaGec(CIZGI_TAKIP);
      break;

    case CIZGI_TAKIP: {               // Cizgiyi izle, onde nesne varsa yaklas
      cizgiTakip(HIZ);
      long cm = mesafeCm();
      if (cm > 0 && cm < 25) adimaGec(YAKLAS);
      break;
    }

    case YAKLAS: {                    // Yavasca nesneye yaklas
      long cm = mesafeCm();
      if (cm == 0) dur();             // Olcum yoksa ilerleme
      else if (cm > YAKIN_CM) sur(60, 60);
      else { dur(); adimaGec(AL); }
      break;
    }

    case AL:                          // Kolu kapat, biraz geri cekil
      kol.write(SERVO_KAPALI);
      if (adimSuresi() > 600) sur(-60, -60);
      if (adimSuresi() > 1000) { dur(); adimaGec(DONUS); }
      break;

    case DONUS:                       // Yerinde 180 derece don
      sur(DONUS_HIZI, -DONUS_HIZI);
      if (adimSuresi() > (unsigned long)DONUS_MS * 2) { dur(); adimaGec(BIRAK); }
      break;

    case BIRAK:                       // Hedef: 4 sensor de siyah (genis bant)
      if (gorenSensor == 4) {
        dur();
        kol.write(SERVO_ACIK);
        adimaGec(BITTI);
      } else {
        cizgiTakip(70);
      }
      break;

    case BITTI:
      dur();
      break;
  }
  delay(10);
}
