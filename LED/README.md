# Aula 01: Piscando LEDs - Placa Didática ESP32

Este é o primeiro projeto de exemplo para a placa didática com ESP32, demonstrando como piscar três LEDs, utilizando apenas uma led_task.


## Configuração do Hardware

Para este código funcionar, é necessário posicionar os jumpers corretamente na placa:

1.  Conecte o jumper do **LD1** ao pino **GPIO 16**.
2.  Conecte o jumper do **LD2** ao pino **GPIO 17**.
2.  Conecte o jumper do **LD3** ao pino **GPIO 18**.

## Como Compilar e Gravar

Este projeto foi feito com o ESP-IDF. Para compilar e gravar, use os seguintes comandos no terminal do ESP-IDF:

```bash
# Limpa compilações antigas (opcional, mas bom)
idf.py fullclean

# Compila o projeto
idf.py build

# Grava na placa (substitua COMx pela sua porta)
idf.py -p COMx flash

# Abre o monitor serial para ver os logs
idf.py -p COMx monitor