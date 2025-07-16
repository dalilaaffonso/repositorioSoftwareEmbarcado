#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dht11.h" // A inclusão está correta

// Define o TAG para as mensagens de log
static const char *TAG = "DHT11_TEST";

// Define o pino GPIO onde o DHT11 está conectado
#define DHT11_PIN GPIO_NUM_17

void dht11_read_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Iniciando a leitura do sensor DHT11...");

    // Neste driver, não há função de inicialização.
    // Em vez disso, criamos uma estrutura e definimos o pino.
    dht11_t dht11_sensor;
    dht11_sensor.dht11_pin = DHT11_PIN;

    while(1) {
        // A função de leitura se chama 'dht11_read' (minúsculas)
        // e retorna um inteiro (0 para sucesso, -1 para erro).
        // Passamos o endereço da nossa estrutura para que ela seja preenchida.
        int result = dht11_read(&dht11_sensor, 10);

        if (result == 0) { // Verificamos o status pelo retorno da função
            // Se a leitura foi bem-sucedida, imprime os valores.
            // Os valores são do tipo 'float', então usamos '%.1f' para formatar.
            ESP_LOGI(TAG, "Temperatura: %.1f°C | Umidade: %.1f%%", dht11_sensor.temperature, dht11_sensor.humidity);
        } else {
            // Se a leitura falhou, avisa no log.
            ESP_LOGW(TAG, "Falha ao ler o sensor DHT11.");
        }

        // Espera 2 segundos antes da próxima leitura.
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "App principal iniciada. Criando task para o DHT11.");

    xTaskCreate(
        dht11_read_task,
        "DHT11 Read Task",
        2048,
        NULL,
        5,
        NULL
    );
}