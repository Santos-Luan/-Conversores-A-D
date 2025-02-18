# README

## Descrição

Este projeto é desenvolvido para o Raspberry Pi Pico e utiliza diversos periféricos, incluindo I2C, PWM e ADC. O código foi projetado para interagir com um joystick (utilizando os pinos de EIXO X e Y) e botões, além de controlar LEDs e exibir informações em um display OLED SSD1306. A aplicação realiza a leitura das posições do joystick para controlar a intensidade de LEDs, além de exibir as coordenadas do joystick no display OLED e desenhar uma "casinha" de forma simples.

## Componentes Utilizados

- **Raspberry Pi Pico** (Placa microcontroladora)
- **Display OLED 128x64** com interface I2C
- **Joystick** (utilizado para controlar a intensidade dos LEDs e coordenadas X e Y)
- **LEDs** (Azul, Vermelho, Verde) controlados por PWM
- **Botões** (Joystick, Botão A, Botão BOOTSEL)

## Conexões

- **I2C**:
  - SDA: GPIO 14
  - SCL: GPIO 15
- **Joystick**:
  - EIXO X: GPIO 26 (ADC0)
  - EIXO Y: GPIO 27 (ADC1)
  - BOTÃO JOYSTICK: GPIO 22
- **Botões**:
  - BOTÃO A: GPIO 5
  - BOTÃO BOOTSEL: GPIO 6
- **LEDs**:
  - LED Azul: GPIO 12
  - LED Vermelho: GPIO 13
  - LED Verde: GPIO 11

## Funcionalidades

- **Joystick**: O valor lido nos eixos X e Y é utilizado para controlar a intensidade dos LEDs Azul e Vermelho. Se o joystick estiver em uma posição centralizada, os LEDs serão desligados.
- **Botão Joystick**: Alterna o estado do LED Verde (ligado/desligado).
- **Botão A**: Desativa os LEDs Vermelho e Azul quando pressionado.
- **Botão BOOTSEL**: Realiza um reset no dispositivo, colocando-o em modo de boot.

## Características do Código

- **Debounce**: Implementação de debounce para evitar leituras erradas nos botões.
- **PWM**: Controle da intensidade dos LEDs Azul, Vermelho e Verde via PWM.
- **Display OLED**: Exibe os valores dos eixos X e Y, além de desenhar uma casinha simples como forma de interação visual.
- **Desenho no Display**: Desenha uma casinha com linha, criando um efeito visual representativo.

## Estrutura do Código

### Funções

- **tratar_interrupcao_gpio**: Função para lidar com as interrupções dos botões, acionando mudanças de estado nos LEDs e realizando o reset via BOOTSEL.
- **configurar_pwm**: Configura o PWM para os LEDs.
- **atualizar_display**: Atualiza o display OLED com os valores de X e Y lidos do joystick.
- **debounce**: Implementa o controle de debounce para garantir que os botões não sejam acionados múltiplas vezes de forma errada.
- **calcular_pwm**: Calcula o valor de PWM a ser aplicado nos LEDs, dependendo da posição do joystick.
- **desenhar**: Desenha uma casinha simples no display OLED.

### Interrupções

Através da função `gpio_set_irq_enabled_with_callback`, as interrupções são configuradas para os botões, permitindo uma resposta rápida às ações do usuário.

## Como Compilar e Executar

1. **Configuração do Ambiente**:
   - Certifique-se de ter o SDK do Raspberry Pi Pico instalado corretamente.
   - Configure o ambiente de desenvolvimento para C no Raspberry Pi Pico.

2. **Compilação**:
   Compile o código utilizando CMake para gerar o firmware para o Raspberry Pi Pico.

3. **Execução**:
   - Transfira o binário gerado para o Raspberry Pi Pico.
   - Conecte o Raspberry Pi Pico ao computador ou à alimentação.

4. **Testes**:
   - Verifique as leituras do joystick no display OLED.
   - Acione os botões e observe a mudança nos LEDs e no comportamento do sistema.
