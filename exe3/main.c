#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int buffer[5] = {0}; // Buffer para armazenar os últimos 5 valores
    int index = 0; // Índice para o buffer circular
    int sum = 0; // Soma dos valores no buffer


    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {

            sum -= buffer[index];
            sum += data;
            buffer[index] = data;

            // Calcula a média móvel e imprime o resultado
            if (index >= 4) { // Garante que temos 5 valores para a média
                int avg = sum / 5;
                printf("Média móvel: %d\n", avg);
            }

            // Atualiza o índice para o próximo elemento
            index = (index + 1) % 5;


            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
