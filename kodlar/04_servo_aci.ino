// 4. SERVO: Seri Monitor'den aci yaz, servo o aciya gitsin
// Sinyal -> 13. Seri Monitor: 9600 baud, satir sonu "Yeni satir".

#include <Servo.h>

Servo kol;
const byte SERVO_PIN = 13;
const int EN_AZ = 10, EN_COK = 170;   // Guvenli sinirlar; mekanizmaya gore daraltin

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  kol.attach(SERVO_PIN);
  kol.write(90);
  Serial.println("Bir aci yazip Enter'a basin (10-170):");
}

void loop() {
  if (Serial.available()) {
    int aci = Serial.parseInt();
    while (Serial.available()) Serial.read();   // Satir sonunu temizle

    if (aci >= EN_AZ && aci <= EN_COK) {
      kol.write(aci);
      Serial.print("Servo: ");
      Serial.println(aci);
    } else {
      Serial.println("10 ile 170 arasinda bir sayi yazin.");
    }
  }
}
