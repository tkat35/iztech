// 2. CIZGI SENSORU: dort gozu oku, cizginin nerede oldugunu yaz
// OUT1-OUT4 soldan saga A0-A3. Seri Monitor: 9600 baud.

const byte CIZGI[4] = {A0, A1, A2, A3};
const bool SIYAH_LOW = true;  // Siyahta 0 okunuyorsa true, 1 okunuyorsa false yapin

bool cizgiVar(byte pin) {
  int d = digitalRead(pin);
  return SIYAH_LOW ? d == LOW : d == HIGH;
}

void setup() {
  Serial.begin(9600);
  for (byte i = 0; i < 4; i++) pinMode(CIZGI[i], INPUT);
}

void loop() {
  bool s[4];
  for (byte i = 0; i < 4; i++) {
    s[i] = cizgiVar(CIZGI[i]);
    Serial.print(s[i] ? "# " : ". ");   // # = siyah, . = beyaz
  }

  int sol = s[0] + s[1];
  int sag = s[2] + s[3];
  Serial.print("  -> ");
  if (sol + sag == 0)      Serial.println("cizgi yok");
  else if (sol + sag == 4) Serial.println("genis bant ya da kavsak");
  else if (sol > sag)      Serial.println("cizgi solda");
  else if (sag > sol)      Serial.println("cizgi sagda");
  else                     Serial.println("cizgi ortada");

  delay(150);
}
