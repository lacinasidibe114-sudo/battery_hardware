#include <Wire.h>
#include <Adafruit_INA226.h>

Adafruit_INA226 ina226;
unsigned long last_time = 0;
const unsigned long interval = 100; // 10 Hz

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Adresse I2C par défaut (souvent 0x40)
  if (!ina226.begin(0x40)) {
    Serial.println("Erreur : INA226 non connecté.");
    while (1);
  }
  
  ina226.setAverage(INA226_AVERAGE_16);
  
  // Attention : Vérifie la valeur du shunt sur ta carte !
  // Si c'est un R100, c'est 0.1 Ohm. Si c'est un R002, c'est 0.002 Ohm.
  ina226.setResistance(0.1); // 0.1 Ohm
  ina226.setMaxCurrent(3.2); // 3.2 A max
  
  Serial.println("INA226 prêt !");
}

void loop() {
  unsigned long current_time = millis();
  
  if (current_time - last_time >= interval) {
    float bus_voltage = ina226.getBusVoltage(); // Tension (V)
    float current_A = ina226.getCurrent_mA() / 1000.0; // Courant (A)
    float power_W = ina226.getPower_mW() / 1000.0;     // Puissance (W)
    
    // Envoi trame : "V:12.45 I:1.250 P:15.56"
    Serial.print("V:"); Serial.print(bus_voltage, 2);
    Serial.print(" I:"); Serial.print(current_A, 3);
    Serial.print(" P:"); Serial.println(power_W, 2);
    
    last_time = current_time;
  }
}