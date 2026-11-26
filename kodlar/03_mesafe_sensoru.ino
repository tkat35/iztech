// 3. MESAFE SENSORU (HC-SR04 / HY-SRF05): olc, ortalamasini al, cubuk ciz
// TRIG -> 11, ECHO -> 12. Seri Monitor: 9600 baud.

const byte TRIG = 11, ECHO = 12;

long olc() {
  digitalWrite(TRIG, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  unsigned long sure = pulseIn(ECHO, HIGH, 25000);  // En fazla 25 ms bekle
  return sure == 0 ? 0 : sure / 58;                 // 0 = olcum yok
}

// 5 olcumun ortalamasi. Olcum yoksa (0) ortalamaya katilmaz.
long mesafeCm() {
  long toplam = 0;
  int n = 0;
  for (int i = 0; i < 5; i++) {
    long cm = olc();
    if (cm > 0) { toplam += cm; n++; }
    delay(10);
  }
  return n > 0 ? toplam / n : 0;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  long cm = mesafeCm();
  if (cm == 0) {
    Serial.println("olcum yok (cok uzak ya da egik yuzey)");
  } else {
    Serial.print(cm);
    Serial.print(" cm  ");
    long cubuk = cm > 60 ? 30 : cm / 2;   // Her = isareti 2 cm
    for (long i = 0; i < cubuk; i++) Serial.print('=');
    Serial.println('|');
  }
  delay(200);
}
