// 2. CIZGI SENSORU (ALPGEN M052): uc gozu oku, cizginin nerede oldugunu yaz
// Sol -> 2, Orta -> 3, Sag -> 4. Seri Monitor: 9600 baud.

const byte CIZGI[3] = {2, 3, 4};   // M052: sol, orta, sag
const bool SIYAH_LOW = true;       // Siyahta 0 okunuyorsa true, 1 okunuyorsa false yapin

bool cizgiVar(byte pin) {
  int d = digitalRead(pin);
  return SIYAH_LOW ? d == LOW : d == HIGH;
}

void setup() {
  Serial.begin(9600);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
}

void loop() {
  bool s[3];
  for (byte i = 0; i < 3; i++) {
    s[i] = cizgiVar(CIZGI[i]);
    Serial.print(s[i] ? "# " : ". ");   // # = siyah, . = beyaz
  }

  Serial.print("  -> ");
  if (!s[0] && !s[1] && !s[2])    Serial.println("cizgi yok");
  else if (s[0] && s[1] && s[2])  Serial.println("genis bant ya da kavsak");
  else if (s[0] && !s[2])         Serial.println("cizgi solda");
  else if (s[2] && !s[0])         Serial.println("cizgi sagda");
  else                            Serial.println("cizgi ortada");

  delay(150);
}
