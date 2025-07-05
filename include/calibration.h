#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Preferences.h> // Inclui a biblioteca Preferences para NVS na ESP32

// Define a direção da mudança de carga para compensação de histerese
enum class LoadDirection {
    ASCENDING,  // Carga aumentando
    DESCENDING, // Carga diminuindo
    STABLE      // Carga estável ou mudança insignificante
};

// Estrutura para armazenar os coeficientes de calibração (slope e offset)
struct CalibrationCoefficients {
    double slope;
    double offset;

    // Construtor para fácil inicialização
    CalibrationCoefficients(double s = 0.0, double o = 0.0) : slope(s), offset(o) {}
};

class Calibration {
private:
    Preferences preferences; // Objeto Preferences para acessar o NVS da ESP32
    const char* _nvs_namespace_key; // Armazena o nome único do namespace para esta instância

    // Chaves para as entradas no NVS
    static constexpr const char* NVS_NAMESPACE = "balanca_cal"; // Namespace para suas chaves de calibração
    static constexpr const char* NVS_KEY_SLOPE_ASC = "slope_asc";
    static constexpr const char* NVS_KEY_OFFSET_ASC = "offset_asc";
    static constexpr const char* NVS_KEY_SLOPE_DESC = "slope_desc";
    static constexpr const char* NVS_KEY_OFFSET_DESC = "offset_desc";
    static constexpr const char* NVS_KEY_ZERO_ADC_REF = "zero_adc_ref";
    static constexpr const char* NVS_KEY_CALIBRATED = "calibrated"; // Flag para indicar se a balança já foi calibrada

    // Valores padrão de fábrica/primeira inicialização, se NVS não tiver dados
    static constexpr const double DEFAULT_SLOPE_ASC = 0.01;
    static constexpr const double DEFAULT_OFFSET_ASC = 0.0;
    static constexpr const double DEFAULT_SLOPE_DESC = 0.01;
    static constexpr const double DEFAULT_OFFSET_DESC = 0.0;
    static constexpr const int DEFAULT_ZERO_ADC_REF = 0;

public:
    // Coeficientes de calibração para o caminho ascendente (carga aumentando)
    CalibrationCoefficients ascending_coeffs;
    // Coeficientes de calibração para o caminho descendente (carga diminuindo)
    CalibrationCoefficients descending_coeffs;
    // Leitura ADC de referência para o ponto zero (sem carga)
    int zero_adc_reference;

    // Limiares para autocalibração
    double zero_drift_threshold_adc; // Limiar de desvio para autocalibração de zero em unidades ADC
    double reference_weight_drift_threshold_grams; // Limiar de desvio para ajuste de ponto único em gramas

    // Construtor
    Calibration(
        const char* nvs_namespace_key,
        double initial_slope_asc = 0.0, double initial_offset_asc = 0.0,
        double initial_slope_desc = 0.0, double initial_offset_desc = 0.0,
        int initial_zero_adc_ref = 0,
        double zero_thresh_adc = 50.0, double ref_weight_thresh_grams = 2.0);

    // Aplica a calibração e retorna o peso corrigido
    double applyCalibration(int raw_adc_reading, LoadDirection direction) const;

    // Realiza a autocalibração do ponto zero
    void performZeroCalibration(int current_raw_adc_reading);

    // Realiza um ajuste de calibração usando um peso de referência conhecido
    void performReferencePointCalibration(int current_raw_adc_reading, double known_weight_grams, LoadDirection direction);

    // Métodos para persistência de dados usando Preferences (NVS)
    void saveCalibrationData();
    void loadCalibrationData();
};

#endif // CALIBRATION_H
