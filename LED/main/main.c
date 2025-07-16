#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Define um TAG para as mensagens de log
static const char *TAG = "LED_APP";


#define LED1 GPIO_NUM_16 
#define LED2 GPIO_NUM_17 
#define LED3 GPIO_NUM_18


void led_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Task LED iniciada. Configurando os GPIOs %d, %d e %d...", LED1, LED2, LED3);

    // Configura o pino do LED1
    gpio_reset_pin(LED1);
    gpio_set_direction(LED1, GPIO_MODE_OUTPUT);

    // Configura o pino do LED2
    gpio_reset_pin(LED2);
    gpio_set_direction(LED2, GPIO_MODE_OUTPUT);

    // Configura o pino do LED3
    gpio_reset_pin(LED3);
    gpio_set_direction(LED3, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "GPIOs configurados. Iniciando o loop de piscar.");

    while (1) {
        // Acende o LED 1 
        gpio_set_level(LED1, 1);
        gpio_set_level(LED2, 0);
        gpio_set_level(LED3, 0);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Pausa de 1 segundo

        // Acende o LED 2
        gpio_set_level(LED1, 0);
        gpio_set_level(LED2, 1);
        gpio_set_level(LED3, 0);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Pausa de 1 segundo

        // Acende o LED 3
        gpio_set_level(LED1, 0);
        gpio_set_level(LED2, 0);
        gpio_set_level(LED3, 1);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Pausa de 1 segundo
    }
}


void app_main(void)
{
    ESP_LOGI(TAG, "App principal iniciada. Criando task para piscar os LEDs.");

    // Cria a task que vai efetivamente piscar os LEDs
    xTaskCreate(
        led_task,     // Função da task
        "led_task",   // Nome da task (para debug)
        2048,               // Tamanho da pilha (stack)
        NULL,               // Parâmetros (nenhum)
        5,                  // Prioridade
        NULL                // Handle (não precisamos)
    );
}