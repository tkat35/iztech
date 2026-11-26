// 1. MOTORLAR: duz git, 90 derece don, yine duz git
// Pinler sayfadaki ortak baglanti tablosuna goredir.
// Acildiktan 3 sn sonra bir kez calisir. Tekrar icin RESET dugmesine basin.

const byte SOL_PWM = 5, SOL_IN1 = 7, SOL_IN2 = 8;
const byte SAG_PWM = 6, SAG_IN1 = 9, SAG_IN2 = 10;

const int HIZ = 120;        // 0-255 arasi
const int SAG_AYAR = 0;     // Robot sola kayiyorsa eksi (-10), saga kayiyorsa arti (+10)
const int ILERI_MS = 1500;  // Ileri gitme suresi (ms)
const int DONUS_MS = 450;   // Tam 90 derece donene kadar bu sayiyi degistirin

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

void setup() {
  pinMode(SOL_PWM, OUTPUT); pinMode(SOL_IN1, OUTPUT); pinMode(SOL_IN2, OUTPUT);
  pinMode(SAG_PWM, OUTPUT); pinMode(SAG_IN1, OUTPUT); pinMode(SAG_IN2, OUTPUT);
  sur(0, 0);
  delay(3000);                                  // Robotu yere koymak icin zaman

  sur(HIZ, HIZ + SAG_AYAR);  delay(ILERI_MS);   // Ileri
  sur(0, 0);                 delay(500);
  sur(HIZ, -HIZ);            delay(DONUS_MS);   // Yerinde saga don
  sur(0, 0);                 delay(500);
  sur(HIZ, HIZ + SAG_AYAR);  delay(ILERI_MS);   // Yine ileri
  sur(0, 0);
}

void loop() {
  // Bos: hareket bir kez yapildi.
}
