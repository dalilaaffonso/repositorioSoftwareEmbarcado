#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// Define um TAG para as mensagens de log
static const char *TAG = "LM35_TEST";

// --- Configurações do ADC ---
// O LM35 está conectado no GPIO35, que corresponde ao Canal 7 do ADC1
#define LM35_ADC_CHANNEL ADC_CHANNEL_7
#define ADC_ATTENUATION ADC_ATTEN_DB_11 // Atenuação para medir tensões até aprox. 2.5V

// Variáveis globais para os "handles" do ADC
adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t cali_handle = NULL;


// Função para configurar e calibrar o ADC
static void configure_adc(void)
{
    // --- Configuração da Unidade ADC ---
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    // --- Configuração do Canal ADC ---
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Largura de bits padrão (geralmente 12 bits)
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, LM35_ADC_CHANNEL, &config));

    // --- Calibração do ADC ---
    // O ADC do ESP32 não é perfeitamente linear, a calibração melhora a precisão.
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Calibração do ADC falhou, usando valores brutos.");
    }
}

void lm35_read_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Task de leitura do LM35 iniciada.");
    
    int adc_raw;
    int voltage_mv;

    while(1) {
        // 1. Lê o valor "cru" do ADC
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, LM35_ADC_CHANNEL, &adc_raw));

        // 2. Converte o valor "cru" para uma tensão em milivolts (mV)
        if (cali_handle != NULL) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_mv));
        } else {
            // Se a calibração não funcionou, fazemos uma estimativa simples (menos precisa)
            voltage_mv = adc_raw * 2500 / 4095; 
        }

        // 3. Converte a tensão em temperatura
        // O sensor LM35 gera 10mV para cada 1 grau Celsius
        float temperature_c = (float)voltage_mv / 10.0;
        
        ESP_LOGI(TAG, "Valor Raw ADC: %d, Tensão: %d mV, Temperatura: %.1f °C", adc_raw, voltage_mv, temperature_c);

        // Espera 2 segundos para a próxima leitura
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "App principal iniciada. Configurando o ADC...");

    // Chama a função que prepara o ADC para uso
    configure_adc();
    
    // Cria a task que fará a leitura contínua do sensor
    xTaskCreate(lm35_read_task, "LM35 Read Task", 4096, NULL, 5, NULL);
}