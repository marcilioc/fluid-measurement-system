/*
  HX711 Calibration
  Caio Marcilio dos Santos
  Arduino UNO
*/

#include "HX711.h"  // HX711 library
#include "stdio.h"  // Miscelaneous library
#include <driver/adc.h>  // ESP32 ADC Driver

#define DOUT A4  // HX711 DATA OUT - ESP32 Pin D32 pin
#define CLK A5   // HX711 SCK IN   - ESP32 Pin D33 pin

HX711 scale;  // Scale library instance

float calibrationFactor = 97980;  // Initial calibration factor

void setup() {
  // ADCs configurarion
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_0);

  // Serial configuration
  Serial.begin(115200);
  scale.begin(DOUT, CLK);
  Serial.println();
  Serial.println("Calibração do HX711");
  Serial.println("Remova qualquer peso da balança");
  Serial.println("Depois que as leituras começarem, coloque um peso conhecido sobre a balança");
  Serial.println("Pressione a, s, d, f para aumentar Fator de Calibração por 10, 100, 1000, 10000 respectivamente");
  Serial.println("Pressione z, x, c, v para diminuir Fator de Calibração por 10, 100, 1000, 10000 respectivamente");
  Serial.println("Após leitura correta do peso, pressione t para TARA (zerar) ");

  // Configure and start scale
  scale.set_scale();  
  setScaleZero();
}

void loop() {
  scale.set_scale(calibrationFactor);
  Serial.print("Peso: ");
  Serial.print(scale.get_units(10));
  Serial.print(" kg");
  Serial.print("    Fator de Calibração: ");
  Serial.println(calibrationFactor);
  delay(100);

  // Calibration Factor Correction
  if (Serial.available()) {
    char temp = Serial.read();
    switch (temp) {
      case 'a':
        calibrationFactor += 10;
        break;
      case 'z':
        calibrationFactor -= 10;
        break;
      case 's':
        calibrationFactor += 100;
        break;
      case 'x':
        calibrationFactor -= 100;
        break;
      case 'd':
        calibrationFactor += 1000;
        break;
      case 'c':
        calibrationFactor -= 1000;
        break;
      case 'f':
        calibrationFactor += 10000;
        break;
      case 'v':
        calibrationFactor -= 10000;
        break;
      case 't':
        setScaleZero();
        break;
    }
  }
}

void setScaleZero() {
  Serial.println();
  scale.tare();  // Set new scale zero
  Serial.println("New scale zero setted");
}
