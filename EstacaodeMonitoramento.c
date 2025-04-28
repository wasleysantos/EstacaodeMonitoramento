/*
Aluno: Wasley dos Santos Silva
Matricula: 20251RSE.MTC0037

Atividade: Estação de Monitoramento Interativo
Criar um sistema de dois núcleos que simula uma estação de monitoramento
com sensores e atuadores.

Definição de estados do Joystick: 

***Atividade Alta = Joystick pra cima 
***Atividade Nutra = Joystick sem uso 
**Atividade Moderada = Joystick no meio e para os lados direito e esquerdo
*Atividade Baixa = Joystick pra baixo

*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"

// Definições dos pinos
#define JOYSTICK_PIN 26    // Pino ADC0 conectado ao joystick
#define BUZZER_PIN 10      // Pino do buzzer
#define LED_R_PIN 13       // Pino do LED vermelho
#define LED_G_PIN 11       // Pino do LED verde
#define LED_B_PIN 12       // Pino do LED azul

// Limiar para os diferentes estados do joystick
#define LIMIAR_CRITICO 4000 //Limiar do joystick para cima
#define LIMIAR_INTERMEDIARIO 1700 //Limiar do joystick para os lados
#define LIMIAR_BAIXO 100 //Limiar do joystick para baixo
#define LIMIAR_NEUTRO 2050 //intervalo 0 da Limiar do joystick centralizado
#define LIMIAR_NEUTRO1 2100 //intervalo 1 da Limiar do joystick centralizado

// Variáveis globais
volatile uint16_t flag_estado = 0;
volatile bool buzzer_ativo = false;
volatile uint32_t buzzer_timer = 0;
volatile bool beep_periodico = false;

// Função para configurar o hardware
void configurar_hardware() {
    adc_init();
    adc_gpio_init(JOYSTICK_PIN);
    adc_select_input(0);

    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_init(BUZZER_PIN);

    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0); // Garante que o buzzer comece desligado
}

// Gera o beep periódico (a cada 2 segundos)
void som_periodico() {
    static uint32_t last_beep_time = 0;

    if (time_us_64() >= last_beep_time) {
        last_beep_time = time_us_64() + 2000000; // 2 segundos
        beep_periodico = true; // Dispara um bipe
    }
}

// Toca o buzzer se a atividade for ALTA
void play_buzzer_sound() {
    static bool buzzer_state = false;

    if (buzzer_ativo || beep_periodico) {
        if (buzzer_timer < time_us_64()) {
            buzzer_timer = time_us_64() + 200000; // 200 ms

            buzzer_state = !buzzer_state;
            gpio_put(BUZZER_PIN, buzzer_state);

            // Se o bipe foi periódico, desligamos depois de um toque
            if (beep_periodico && !buzzer_state) {
                beep_periodico = false; // Reseta o beep periódico
            }
        }
    } else {
        gpio_put(BUZZER_PIN, 0);
    }
}

// Ajusta o LED baseado no estado
void ajustar_led(uint16_t estado) {
    if (estado > LIMIAR_CRITICO) {
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);
        printf("Atividade Alta! LED vermelho ligado.\n");
        buzzer_ativo = true;
    } else if (estado >= LIMIAR_NEUTRO && estado <= LIMIAR_NEUTRO1) {
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 1);
        gpio_put(LED_B_PIN, 1);
        printf("Sem Atividade! LED branco ligado.\n");
        buzzer_ativo = false;
    }else if (estado > LIMIAR_INTERMEDIARIO && estado <= LIMIAR_CRITICO) {
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 1);
        printf("Atividade Moderada! LED azul ligado.\n");
        buzzer_ativo = false;
    } else if (estado < LIMIAR_BAIXO) {
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 1);
        gpio_put(LED_B_PIN, 0);
        printf("Atividade Baixa! LED verde ligado.\n");
        buzzer_ativo = false;
    }
}

// Função para enviar o estado a cada 2 segundos pelo FIFO
int64_t enviar_estado_alarme(alarm_id_t id, void *user_data) {
    multicore_fifo_push_blocking(flag_estado);
    printf("[Core 0] Estado enviado via FIFO: %d\n", flag_estado);

    return 2000; // Repetir em 2 segundos
}

//Core 1
void core1_main() {
    while (true) {
        if (multicore_fifo_rvalid()) {
            uint16_t estado_recebido = multicore_fifo_pop_blocking();
            printf("[Core 1] Estado recebido: %d\n", estado_recebido);
            ajustar_led(estado_recebido);
        }
        play_buzzer_sound();
    }
}

// Código principal no Core 0
int main() {
    stdio_init_all();
    configurar_hardware();
    add_alarm_in_ms(2000, enviar_estado_alarme, NULL, true);
    multicore_launch_core1(core1_main);

    while (true) {
        uint16_t leitura = adc_read();
        flag_estado = leitura;

        printf("[Core 0] Leitura do joystick: %d\n", leitura);
        
        som_periodico(); // Gera bipe a cada 2 segundos

        sleep_ms(2000); // Pausa de 2 segundo para próxima leitura
    }
}
