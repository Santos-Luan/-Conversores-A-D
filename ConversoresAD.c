#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"
#include "pico/time.h"
#include "pico/bootrom.h"

#define PORTA_I2C i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define EIXO_X 26         // GPIO para eixo X
#define EIXO_Y 27         // GPIO para eixo Y
#define BOTAO_JOYSTICK 22 // GPIO para botão do joystick
#define BOTAO_A 5         // GPIO para botão A
#define LED_AZUL 12       // LED Azul
#define LED_VERMELHO 13   // LED Vermelho
#define LED_VERDE 11      // LED Verde
#define BOTAO_BOOTSEL 6   // Botão BOOTSEL

ssd1306_t display;
const uint16_t PERIODO_PWM = 4095;
const float DIVISOR_PWM = 16.0;
bool leds_ativos = true;
bool estado_led_verde = false;
bool cor = true;

void tratar_interrupcao_gpio(uint gpio, uint32_t eventos);        // Função para tratar interrupções nos botões
void configurar_pwm(uint gpio);                                   // Configuração do PWM
void atualizar_display(uint16_t x, uint16_t y);                   // Atualiza a exibição no display
void debounce(uint gpio, absolute_time_t *ultimo_pressionamento); // Implementação do debounce
uint16_t calcular_pwm(uint16_t valor);                            // Calcula o nível PWM com base na posição do joystick
void desenhar();

absolute_time_t ultimo_pressionamento_joystick, ultimo_pressionamento_botaoA;

void tratar_interrupcao_gpio(uint gpio, uint32_t eventos)
{
    absolute_time_t agora = get_absolute_time();
    if (gpio == BOTAO_JOYSTICK)
    {
        if (absolute_time_diff_us(ultimo_pressionamento_joystick, agora) > 200000)
        {
            ultimo_pressionamento_joystick = agora;
            estado_led_verde = !estado_led_verde;
            pwm_set_gpio_level(LED_VERDE, estado_led_verde ? PERIODO_PWM : 0);
        }
    }
    if (gpio == BOTAO_A)
    {
        if (absolute_time_diff_us(ultimo_pressionamento_botaoA, agora) > 200000)
        {
            ultimo_pressionamento_botaoA = agora;
            leds_ativos = !leds_ativos;
            if (!leds_ativos)
            {
                pwm_set_gpio_level(LED_VERMELHO, 0);
                pwm_set_gpio_level(LED_AZUL, 0);
            }
        }
    }
    if (gpio == BOTAO_BOOTSEL)
    {
        reset_usb_boot(0, 0);
    }
}

int main()
{
    stdio_init_all();
    gpio_init(BOTAO_JOYSTICK);
    gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(BOTAO_JOYSTICK);
    gpio_set_irq_enabled_with_callback(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &tratar_interrupcao_gpio);

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &tratar_interrupcao_gpio);

    gpio_init(BOTAO_BOOTSEL);
    gpio_set_dir(BOTAO_BOOTSEL, GPIO_IN);
    gpio_pull_up(BOTAO_BOOTSEL);
    gpio_set_irq_enabled_with_callback(BOTAO_BOOTSEL, GPIO_IRQ_EDGE_FALL, true, &tratar_interrupcao_gpio);

    i2c_init(PORTA_I2C, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&display, WIDTH, HEIGHT, false, endereco, PORTA_I2C);
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);

    adc_init();
    adc_gpio_init(EIXO_X);
    adc_gpio_init(EIXO_Y);

    configurar_pwm(LED_AZUL);
    configurar_pwm(LED_VERMELHO);
    configurar_pwm(LED_VERDE);

    while (true)
    {
        adc_select_input(0);
        uint16_t valor_x = adc_read();
        adc_select_input(1);
        uint16_t valor_y = adc_read();

        if (leds_ativos)
        {
            if (valor_x >= 2000 && valor_x <= 2135 && valor_y >= 1870 && valor_y <= 2000)
            {
                pwm_set_gpio_level(LED_VERMELHO, 0);
                pwm_set_gpio_level(LED_AZUL, 0);
            }
            else
            {
                pwm_set_gpio_level(LED_VERMELHO, calcular_pwm(valor_x));
                pwm_set_gpio_level(LED_AZUL, calcular_pwm(valor_y));
            }
        }

        atualizar_display(valor_x, valor_y);

        sleep_ms(100);
    }
}

uint16_t calcular_pwm(uint16_t valor)
{ // Calcula o nível PWM baseado no valor do joystick
    if (valor >= 2048)
    {
        return (valor - 2048) * 2;
    }
    else
    {
        return (2048 - valor) * 2;
    }
}

void configurar_pwm(uint gpio)
{ // Configura o PWM para um determinado pino
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv(slice, DIVISOR_PWM);
    pwm_set_wrap(slice, PERIODO_PWM);
    pwm_set_enabled(slice, true);
}

void atualizar_display(uint16_t x, uint16_t y)
{ // Atualiza as informações do display
    char str_x[5], str_y[5];
    sprintf(str_x, "%d", x);
    sprintf(str_y, "%d", y);
    desenhar();
    ssd1306_draw_string(&display, "EIXO X:", 8, 6);
    ssd1306_draw_string(&display, str_x, 60, 6);
    ssd1306_draw_string(&display, "EIXO Y:", 8, 20);
    ssd1306_draw_string(&display, str_y, 60, 20);
    ssd1306_draw_string(&display, "LUAN E S", 10, 40);
    ssd1306_send_data(&display);
}
// Desenha no display oled
void desenhar()
{

    ssd1306_fill(&display, false);
    ssd1306_fill(&display, !cor);
    ssd1306_rect(&display, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    //====CASINHA====>
    // TELHADO
    ssd1306_line(&display, 3, 30, 123, 30, cor);
    ssd1306_line(&display, 80, 45, 90, 38, cor);
    ssd1306_line(&display, 90, 38, 100, 45, cor);
    // PAREDES
    ssd1306_line(&display, 80, 45, 100, 45, cor);
    ssd1306_line(&display, 80, 58, 100, 58, cor);
    ssd1306_line(&display, 80, 58, 80, 45, cor);
    ssd1306_line(&display, 100, 45, 100, 58, cor);
    // PORTA
    ssd1306_line(&display, 87, 48, 93, 48, cor);
    ssd1306_line(&display, 87, 56, 93, 56, cor);
    ssd1306_line(&display, 93, 56, 93, 48, cor);
    ssd1306_line(&display, 87, 56, 87, 48, cor);
    
}