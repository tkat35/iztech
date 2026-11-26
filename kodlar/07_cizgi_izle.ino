// 7. CIZGI + MOTOR: cizgi izle (mantiksal yontem)
// Cizgi kaybolursa son gordugu tarafa yavasca doner ve arar.
// 1,5 saniyede bulamazsa guvenli durur.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;
const byte CIZGI[3] = {2, 3, 4};   // ALPGEN M052: sol, orta, sag
const bool SIYAH_LOW = true;   // 2. uygulamada buldugunuz deger

const int HIZ = 100;      // Duz giderken
const int YAVAS = 50;     // Donus icindeki teker
const int ARA = 70;       // Cizgi kayipken arama hizi
const unsigned long ARAMA_MS = 1500;

int sonYon = 0;                    // -1 sol, +1 sag, 0 bilinmiyor
unsigned long kayipBaslangic = 0;  // 0 = cizgi kayip degil

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

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  for (byte i = 0; i < 3; i++) pinMode(CIZGI[i], INPUT);
  sur(0, 0);
  delay(3000);
}

void loop() {
  bool s[3];
  for (byte i = 0; i < 3; i++) s[i] = cizgiVar(CIZGI[i]);
  bool goruyor = s[0] || s[1] || s[2];
  if (goruyor) kayipBaslangic = 0;

  if (s[1] && s[0] == s[2]) { sur(HIZ, HIZ);       sonYon = 0;  }  // Ortada: duz
  else if (s[0] && s[1])    { sur(YAVAS, HIZ);     sonYon = -1; }  // Biraz solda
  else if (s[0])            { sur(YAVAS / 2, HIZ); sonYon = -1; }  // Cok solda: sert sola
  else if (s[2] && s[1])    { sur(HIZ, YAVAS);     sonYon = 1;  }  // Biraz sagda
  else if (s[2])            { sur(HIZ, YAVAS / 2); sonYon = 1;  }  // Cok sagda: sert saga
  else {                                                    // Cizgi kayip
    if (kayipBaslangic == 0) kayipBaslangic = millis();
    if (millis() - kayipBaslangic > ARAMA_MS) sur(0, 0);   // Bulamadi: dur
    else if (sonYon < 0) sur(-ARA, ARA);
    else if (sonYon > 0) sur(ARA, -ARA);
    else                 sur(0, 0);
  }
}
