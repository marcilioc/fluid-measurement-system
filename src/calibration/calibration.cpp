#include "calibration.h"
#include <iostream>  // Para debug (Serial.print na ESP32)
#include <cmath>     // Para std::abs

// Construtor
Calibration::Calibration(
    const char* nvs_namespace_key,
    double initial_slope_asc, double initial_offset_asc,
    double initial_slope_desc, double initial_offset_desc,
    int initial_zero_adc_ref,
    double zero_thresh_adc, double ref_weight_thresh_grams)
    : ascending_coeffs(initial_slope_asc, initial_offset_asc),
      descending_coeffs(initial_slope_desc, initial_offset_desc),
      zero_adc_reference(initial_zero_adc_ref),
      zero_drift_threshold_adc(zero_thresh_adc),
      reference_weight_drift_threshold_grams(ref_weight_thresh_grams) {
    
    // Tenta carregar dados de calibração persistidos (NVS)
    loadCalibrationData();

    Serial.print("Calibration initialized. Ascending (Slope: ");
    Serial.print(ascending_coeffs.slope);
    Serial.print(", Offset: ");
    Serial.print(ascending_coeffs.offset);
    Serial.print("), Descending (Slope: ");
    Serial.print(descending_coeffs.slope);
    Serial.print(", Offset: ");
    Serial.print(descending_coeffs.offset);
    Serial.print("), Zero ADC Ref: ");
    Serial.println(zero_adc_reference);
}

// Aplica a calibração com base na direção da carga
double Calibration::applyCalibration(int raw_adc_reading, LoadDirection direction) const {
    double calibrated_weight;
    if (direction == LoadDirection::ASCENDING) {
        calibrated_weight = (static_cast<double>(raw_adc_reading) * ascending_coeffs.slope) + ascending_coeffs.offset;
    } else if (direction == LoadDirection::DESCENDING) {
        calibrated_weight = (static_cast<double>(raw_adc_reading) * descending_coeffs.slope) + descending_coeffs.offset;
    } else { // LoadDirection::STABLE ou outros, pode usar o ascendente como padrão para não haver quebra na leitura
        calibrated_weight = (static_cast<double>(raw_adc_reading) * ascending_coeffs.slope) + ascending_coeffs.offset;
    }
    return calibrated_weight;
}

// Realiza a autocalibração do ponto zero
void Calibration::performZeroCalibration(int current_raw_adc_reading) {
    double desvio_adc = static_cast<double>(current_raw_adc_reading) - zero_adc_reference;

    if (std::abs(desvio_adc) > zero_drift_threshold_adc) {
        double correction = -(static_cast<double>(current_raw_adc_reading) * ascending_coeffs.slope);
        
        ascending_coeffs.offset = correction;
        descending_coeffs.offset = correction;

        zero_adc_reference = current_raw_adc_reading;

        Serial.println("\n--- AUTOCALIBRAÇÃO DE ZERO REALIZADA ---");
        Serial.print("Desvio ADC detectado: ");
        Serial.print(desvio_adc);
        Serial.print(" (limiar: ");
        Serial.print(zero_drift_threshold_adc);
        Serial.println(")");
        Serial.print("Novo Offset (Ascendente/Descendente): ");
        Serial.println(ascending_coeffs.offset);
        Serial.print("Novo Zero ADC Ref: ");
        Serial.println(zero_adc_reference);
        Serial.println("--------------------------------------");

        saveCalibrationData(); // Salva os novos coeficientes no NVS
    }
}

// Realiza um ajuste de calibração usando um peso de referência conhecido
void Calibration::performReferencePointCalibration(int current_raw_adc_reading, double known_weight_grams, LoadDirection direction) {
    double peso_lido_atual;
    CalibrationCoefficients* coeffs_to_adjust;

    if (direction == LoadDirection::ASCENDING) {
        peso_lido_atual = (static_cast<double>(current_raw_adc_reading) * ascending_coeffs.slope) + ascending_coeffs.offset;
        coeffs_to_adjust = &ascending_coeffs;
    } else if (direction == LoadDirection::DESCENDING) {
        peso_lido_atual = (static_cast<double>(current_raw_adc_reading) * descending_coeffs.slope) + descending_coeffs.offset;
        coeffs_to_adjust = &descending_coeffs;
    } else {
        peso_lido_atual = (static_cast<double>(current_raw_adc_reading) * ascending_coeffs.slope) + ascending_coeffs.offset;
        coeffs_to_adjust = &ascending_coeffs;
    }

    double erro_gramas = known_weight_grams - peso_lido_atual;

    if (std::abs(erro_gramas) > reference_weight_drift_threshold_grams) {
        coeffs_to_adjust->offset += erro_gramas; // Ajuste direto do offset

        Serial.println("\n--- AJUSTE COM PESO DE REFERÊNCIA REALIZADO ---");
        Serial.print("Peso de referência real: ");
        Serial.print(known_weight_grams);
        Serial.println("g");
        Serial.print("Peso lido atual: ");
        Serial.print(peso_lido_atual);
        Serial.print("g (usando ");
        Serial.print((direction == LoadDirection::ASCENDING ? "ASCENDING" : "DESCENDING"));
        Serial.println(" coeffs)");
        Serial.print("Erro detectado: ");
        Serial.print(erro_gramas);
        Serial.print("g (limiar: ");
        Serial.print(reference_weight_drift_threshold_grams);
        Serial.println("g)");
        Serial.print("Novo Offset (");
        Serial.print((direction == LoadDirection::ASCENDING ? "Ascendente" : "Descendente"));
        Serial.print("): ");
        Serial.println(coeffs_to_adjust->offset);
        Serial.println("------------------------------------------------");

        saveCalibrationData(); // Salva os novos coeficientes no NVS
    }
}

// Implementação para salvar dados de calibração usando NVS (Preferences)
void Calibration::saveCalibrationData() {
    Serial.println("[CAL] Saving calibration data to NVS...");
    if (preferences.begin(NVS_NAMESPACE, false)) { // Abrir em modo leitura/escrita
        preferences.putDouble(NVS_KEY_SLOPE_ASC, ascending_coeffs.slope);
        preferences.putDouble(NVS_KEY_OFFSET_ASC, ascending_coeffs.offset);
        preferences.putDouble(NVS_KEY_SLOPE_DESC, descending_coeffs.slope);
        preferences.putDouble(NVS_KEY_OFFSET_DESC, descending_coeffs.offset);
        preferences.putInt(NVS_KEY_ZERO_ADC_REF, zero_adc_reference);
        preferences.putBool(NVS_KEY_CALIBRATED, true); // Marca que a balança foi calibrada
        preferences.end(); // Fecha o namespace
        Serial.println("[CAL] Calibration data saved successfully.");
    } else {
        Serial.println("[CAL] Error opening NVS for writing!");
    }
}

// Implementação para carregar dados de calibração usando NVS (Preferences)
void Calibration::loadCalibrationData() {
    Serial.println("[CAL] Loading calibration data from NVS...");
    if (preferences.begin(NVS_NAMESPACE, true)) { // Abrir em modo somente leitura
        bool calibrated_flag = preferences.getBool(NVS_KEY_CALIBRATED, false);

        if (calibrated_flag) {
            ascending_coeffs.slope = preferences.getDouble(NVS_KEY_SLOPE_ASC, DEFAULT_SLOPE_ASC);
            ascending_coeffs.offset = preferences.getDouble(NVS_KEY_OFFSET_ASC, DEFAULT_OFFSET_ASC);
            descending_coeffs.slope = preferences.getDouble(NVS_KEY_SLOPE_DESC, DEFAULT_SLOPE_DESC);
            descending_coeffs.offset = preferences.getDouble(NVS_KEY_OFFSET_DESC, DEFAULT_OFFSET_DESC);
            zero_adc_reference = preferences.getInt(NVS_KEY_ZERO_ADC_REF, DEFAULT_ZERO_ADC_REF);
            Serial.println("[CAL] Calibration data loaded from NVS.");
        } else {
            Serial.println("[CAL] No previous calibration data found in NVS. Using default initial values.");
            // Valores configurados no construtor, apenas para log.
        }
        preferences.end(); // Fecha o namespace
    } else {
        Serial.println("[CAL] Error opening NVS for reading! Using default initial values.");
        // Valores configurados no construtor, apenas para log.
    }
}
