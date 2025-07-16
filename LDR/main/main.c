#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// Define um TAG para as mensagens de log
static const char *TAG = "LDR_TEST";

// --- Configurações do ADC ---
// O LDR está no GPIO34, que corresponde ao Canal 6 do ADC1
#define LDR_ADC_CHANNEL ADC_CHANNEL_7 
#define ADC_ATTENUATION ADC_ATTEN_DB_11 

// Handles para o ADC 
adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t cali_handle = NULL;


// Função para configurar e calibrar o ADC 
static void configure_adc(void)
{
    // Configuração da Unidade ADC1
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    // Configuração do Canal do ADC
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTENUATION,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, LDR_ADC_CHANNEL, &config));

    // Configuração da Calibração
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));
}

void ldr_read_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Task de leitura do LDR iniciada.");
    
    int adc_raw;

    while(1) {
        // 1. Leitura ADC
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, LDR_ADC_CHANNEL, &adc_raw));
        
        // - Mais luz = Menor resistência do LDR = Menor valor lido.
        // - Menos luz = Maior resistência do LDR = Maior valor lido.
        ESP_LOGI(TAG, "Valor Raw do LDR: %d", adc_raw);

        // Espera 1 segundo para a próxima leitura
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "App principal iniciada. Configurando o ADC para o LDR...");
    configure_adc();
    xTaskCreate(ldr_read_task, "LDR Read Task", 4096, NULL, 5, NULL);
}