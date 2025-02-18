#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include <stdlib.h>

#define VRX_PIN 26       // Pino ADC para o eixo X
#define VRY_PIN 27       // Pino ADC para o eixo Y
#define LED_RED_PIN 13   // LED vermelho (PWM)
#define LED_BLUE_PIN 12  // LED azul (PWM)
#define LED_GREEN_PIN 11 // LED verde (acionado pelo botão)
#define BUTTONA_PIN 22    // Botão para LED verde e borda
#define BUTTON_JOYSTICK_PIN 5 // Botão para ativar/desativar LEDs PWM

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

#define DEADZONE_X 50
#define DEADZONE_Y 50
#define AMOSTRAS_DE_CALIBRACAO 100

ssd1306_t ssd;
volatile bool led_state = false;  // Estado do LED verde
volatile bool pwm_enabled = true; // Estado dos LEDs PWM
volatile int borda = 0;           // Estado da borda do display
volatile uint32_t last_button_time = 0;
volatile uint32_t last_pwm_button_time = 0;


// Inicializa PWM em um pino
uint pwm_init_gpio(uint gpio, uint wrap)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Média para calibrar o centro do joystick
uint16_t calibrate_center(uint8_t adc_channel)
{
    uint32_t sum = 0;
    for (int i = 0; i < AMOSTRAS_DE_CALIBRACAO; i++)
    {
        adc_select_input(adc_channel);
        sum += adc_read();
        sleep_ms(5);
    }
    return sum / AMOSTRAS_DE_CALIBRACAO;
}

// Callback do botão do LED verde e troca de borda
void gpio_set_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    if (current_time - last_button_time > 20000) // Debounce
    {
        if (gpio == BUTTONA_PIN)
        {
                // Alterna o estado do LED verde
                led_state = !led_state;
                gpio_put(LED_GREEN_PIN, led_state);
                printf("LED Verde: %s\n", led_state ? "ON" : "OFF");

                // Alterna o estilo da borda
                borda = (borda + 1) % 2; // Alterna entre 0, 1, 2, 3, 4 e 5
                printf("Borda alterada para: %d\n", borda);

                last_button_time = current_time;
            
        }
        else if (gpio == BUTTON_JOYSTICK_PIN)
        {
            pwm_enabled = !pwm_enabled;
            if (!pwm_enabled)
            {
                pwm_set_gpio_level(LED_RED_PIN, 0);
                pwm_set_gpio_level(LED_BLUE_PIN, 0);
            }
            printf("PWM: %s\n", pwm_enabled ? "ON" : "OFF");
            last_pwm_button_time = current_time;
        }
    }
}

int main()
{
    stdio_init_all();

    // Inicializar LEDs
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, 0);

    // Inicializar botões
    gpio_init(BUTTONA_PIN);
    gpio_set_dir(BUTTONA_PIN, GPIO_IN);
    gpio_pull_up(BUTTONA_PIN);

    gpio_init(BUTTON_JOYSTICK_PIN);
    gpio_set_dir(BUTTON_JOYSTICK_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_JOYSTICK_PIN);

    // Inicializar ADC
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);

    // Calibrar centro do joystick
    uint16_t center_x = calibrate_center(0);
    uint16_t center_y = calibrate_center(1);
    printf("Centro X: %u\n", center_x);
    printf("Centro Y: %u\n", center_y);

    // Inicializar PWM nos LEDs
    uint pwm_wrap = 4095;
    pwm_init_gpio(LED_RED_PIN, pwm_wrap);
    pwm_init_gpio(LED_BLUE_PIN, pwm_wrap);

    // Configurar interrupções dos botões
    gpio_set_irq_enabled_with_callback(BUTTONA_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_set_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_set_irq_handler);

    // Inicializar I2C para o display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, 128, 64, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false); // Limpa a tela OLED (preenche com pixels desligados)
    ssd1306_send_data(&ssd);   // Envia os dados para o display

    bool cor = true;
    uint32_t last_print_time = 0;

    while (true)
    {
        // Ler valores do joystick
        adc_select_input(0);
        uint16_t vrx_value = adc_read();
        adc_select_input(1);
        uint16_t vry_value = adc_read();

        if (pwm_enabled)
        {
            int16_t deviation_x = vrx_value - center_x;
            uint16_t valor_pwm_azul = (abs(deviation_x) > DEADZONE_X) ? abs(deviation_x) * 2 : 0;
            if (valor_pwm_azul > pwm_wrap)
                valor_pwm_azul = pwm_wrap;
            pwm_set_gpio_level(LED_BLUE_PIN, valor_pwm_azul);

            int16_t deviation_y = vry_value - center_y;
            uint16_t valor_pwm_vermelho = (abs(deviation_y) > DEADZONE_Y) ? abs(deviation_y) * 2 : 0;
            if (valor_pwm_vermelho > pwm_wrap)
                valor_pwm_vermelho = pwm_wrap;
            pwm_set_gpio_level(LED_RED_PIN, valor_pwm_vermelho);
        }

        // Mapeia os valores do ADC (0-4095) para a tela (3-120 para X e 3-56 para Y)
        int x = 56 - (vrx_value * (64 - 8)) / 4095;
        int y = (vry_value * (128 - 8)) / 4095;

        ssd1306_fill(&ssd, false); // Limpa a tela OLED (preenche com pixels desligados)

        // Desenha a borda de acordo com o estado atual
        switch (borda)
        {
        case 0:
            // Canto arredondado (pequenos segmentos de linha)
            ssd1306_line(&ssd, 6, 3, 119, 3, cor);    // Topo
            ssd1306_line(&ssd, 6, 60, 119, 60, cor);  // Base
            ssd1306_line(&ssd, 3, 6, 3, 57, cor);     // Esquerda
            ssd1306_line(&ssd, 122, 6, 122, 57, cor); // Direita

            // Pequenos cantos curvados
            ssd1306_pixel(&ssd, 4, 4, cor);
            ssd1306_pixel(&ssd, 5, 5, cor);
            ssd1306_pixel(&ssd, 121, 4, cor);
            ssd1306_pixel(&ssd, 120, 5, cor);
            ssd1306_pixel(&ssd, 4, 59, cor);
            ssd1306_pixel(&ssd, 5, 58, cor);
            ssd1306_pixel(&ssd, 121, 59, cor);
            ssd1306_pixel(&ssd, 120, 58, cor);
            break;
        case 1:
            // Borda com cantos em diagonal
            ssd1306_line(&ssd, 3, 3, 10, 3, cor);      // Linha superior esquerda (horizontal)
            ssd1306_line(&ssd, 3, 3, 3, 10, cor);      // Linha superior esquerda (vertical)
            ssd1306_line(&ssd, 122, 3, 115, 3, cor);   // Linha superior direita (horizontal)
            ssd1306_line(&ssd, 122, 3, 122, 10, cor);  // Linha superior direita (vertical)
            ssd1306_line(&ssd, 3, 60, 10, 60, cor);    // Linha inferior esquerda (horizontal)
            ssd1306_line(&ssd, 3, 60, 3, 53, cor);     // Linha inferior esquerda (vertical)
            ssd1306_line(&ssd, 122, 60, 115, 60, cor); // Linha inferior direita (horizontal)
            ssd1306_line(&ssd, 122, 60, 122, 53, cor); // Linha inferior direita (vertical)
            break;
        }

        ssd1306_rect(&ssd, x, y, 8, 8, cor, cor); // Desenha o quadrado na posição corrigida
        ssd1306_send_data(&ssd);                  // Atualiza o display

        // Imprimir valores a cada 1 segundo
        uint32_t current_time = to_us_since_boot(get_absolute_time());
        if (current_time - last_print_time >= 1000000)
        {
            printf("VRX (X-axis): %u\n", vrx_value);
            printf("VRY (Y-axis): %u\n", vry_value);
            printf("PWM Enabled: %s\n", pwm_enabled ? "ON" : "OFF");
            printf("LED Verde: %s\n", led_state ? "ON" : "OFF");
            printf("Borda: %d\n", borda);
            last_print_time = current_time;
        }



        sleep_ms(30);
    }

    return 0;
}