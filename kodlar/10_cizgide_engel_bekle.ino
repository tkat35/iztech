// 10. CIZGI + MESAFE + MOTOR: cizgiyi izle, onde engel varsa bekle
// Engel (el, kutu) kalkinca robot yoluna devam eder.
// Mesafe 60 ms'de bir olculur; cizgi her dongude okunur.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte TRIG = 11, ECHO = 12;
const byte CIZGI[4] = {A0, A1, A2, A3};
const bool SIYAH_LOW = true;          // 2. uygulamadaki deger

const int HIZ = 90, YAVAS = 45, ARA = 70;   // 7. uygulamada ayarladiginiz degerler
const int DUR_CM = 15;
const unsigned long ARAMA_MS = 1500;

int sonYon = 0;
unsigned long kayipBaslangic = 0;
unsigned long sonOlcum = 0;
bool engelVar = false;

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
  unsigned long sure = pulseIn(ECHO, HIGH, 6000);   // En fazla ~1 m: dongu yavaslamasin
  return sure == 0 ? 0 : sure / 58;
}

bool cizgiVar(byte pin) {
  int d = digitalRead(pin);
  return SIYAH_LOW ? d == LOW : d == HIGH;
}

void cizgiIzle() {
  bool s[4];
  for (byte i = 0; i < 4; i++) s[i] = cizgiVar(CIZGI[i]);
  if (s[0] || s[1] || s[2] || s[3]) kayipBaslangic = 0;

  if (s[1] && s[2])  { sur(HIZ, HIZ);       sonYon = 0;  }
  else if (s[0])     { sur(YAVAS / 2, HIZ); sonYon = -1; }
  else if (s[1])     { sur(YAVAS, HIZ);     sonYon = -1; }
  else if (s[3])     { sur(HIZ, YAVAS / 2); sonYon = 1;  }
  else if (s[2])     { sur(HIZ, YAVAS);     sonYon = 1;  }
  else {
    if (kayipBaslangic == 0) kayipBaslangic = millis();
    if (millis() - kayipBaslangic > ARAMA_MS) sur(0, 0);
    else if (sonYon < 0) sur(-ARA, ARA);
    else if (sonYon > 0) sur(ARA, -ARA);
    else                 sur(0, 0);
  }
}

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  for (byte i = 0; i < 4; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  delay(3000);
}

void loop() {
  if (millis() - sonOlcum >= 60) {    // Mesafeyi 60 ms'de bir olc
    sonOlcum = millis();
    long cm = olc();
    engelVar = (cm > 0 && cm < DUR_CM);
  }

  if (engelVar) sur(0, 0);            // Engel var: bekle
  else cizgiIzle();
}
