// Pin definitions
#define LDR1_PIN A0
#define LDR2_PIN A2

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read LDR pins
  int ldr1 = analogRead(LDR1_PIN);
  int ldr2 = analogRead(LDR2_PIN);

  // Output readings
  Serial.print("LDR1: ");
  Serial.print(ldr1);
  Serial.print(" | LDR2: ");
  Serial.println(ldr2);

  delay(100);
}
