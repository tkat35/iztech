// 8. SERVO + MESAFE: radar
// Servo uzerindeki mesafe sensoru 5 yone bakar, en acik yonu yazar.
// Motorlar calismaz; robot masada durabilir. Seri Monitor: 9600 baud.

#include <Servo.h>

const byte TRIG = 11, ECHO = 12;
const byte SERVO_PIN = 3;
const int ACILAR[5] = {30, 60, 90, 120, 150};   // 30 = sag, 150 = sol (montaja gore degisebilir)

Servo kafa;

long olc() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long sure = pulseIn(ECHO, HIGH, 25000);
  return sure == 0 ? 0 : sure / 58;
}

long mesafeCm() {
  long toplam = 0;
  int n = 0;
  for (int i = 0; i < 3; i++) {
    long cm = olc();
    if (cm > 0) { toplam += cm; n++; }
    delay(10);
  }
  return n > 0 ? toplam / n : 0;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  kafa.attach(SERVO_PIN);
  kafa.write(90);
  delay(500);
}

void loop() {
  int enIyiAci = 90;
  long enUzak = 0;

  for (int i = 0; i < 5; i++) {
    kafa.write(ACILAR[i]);
    delay(300);                    // Servo yerine otursun, sonra olc
    long cm = mesafeCm();
    Serial.print(ACILAR[i]);
    Serial.print(" derece: ");
    if (cm == 0) Serial.println("olcum yok (uzak sayildi)");
    else { Serial.print(cm); Serial.println(" cm"); }
    long deger = cm == 0 ? 400 : cm;    // 9. uygulamadaki gibi: olcum yok = acik
    if (deger > enUzak) { enUzak = deger; enIyiAci = ACILAR[i]; }
  }

  Serial.print("En acik yon: ");
  Serial.print(enIyiAci);
  if (enIyiAci < 90)      Serial.println(" (sag)");
  else if (enIyiAci > 90) Serial.println(" (sol)");
  else                    Serial.println(" (on)");
  Serial.println("----");

  kafa.write(90);
  delay(1500);
}
