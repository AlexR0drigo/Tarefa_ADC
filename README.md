#  Tarefa - ADC 

Este projeto utiliza o microcontrolador RP2040 para controlar LEDs RGB e exibir a posição de um joystick em um display SSD1306. O joystick fornece valores analógicos que são convertidos pelo ADC e usados para ajustar a intensidade dos LEDs via PWM, além de mover um quadrado na tela do display. O projeto também implementa interrupções para tratar os botões, alternando o estado do LED verde e a borda do display, além de ativar/desativar os LEDs PWM.

## Componentes Utilizados
Microcontrolador: Raspberry Pi Pico (RP2040)

Joystick: Eixos X e Y conectados aos GPIOs 26 e 27 (ADC)

LEDs RGB:

LED Vermelho: GPIO 13 (PWM)

LED Azul: GPIO 12 (PWM)

LED Verde: GPIO 11 (digital)

Botões:

Botão do Joystick: GPIO 22

Botão A: GPIO 5

Display: SSD1306 (128x64) conectado via I2C (GPIO 14 - SDA, GPIO 15 - SCL)

## Funcionalidades Implementadas
Controle de LEDs RGB via Joystick:

O eixo X controla a intensidade do LED Vermelho.

O eixo Y controla a intensidade do LED Azul.

Quando o joystick está na posição central, os LEDs permanecem apagados. Movimentos para os extremos aumentam o brilho proporcionalmente.

Display SSD1306:

Um quadrado de 8x8 pixels é movido na tela conforme a posição do joystick.

A borda do display alterna entre dois estilos quando o botão do joystick é pressionado.

Interrupções para Botões:

Botão do Joystick: Alterna o estado do LED Verde e muda o estilo da borda do display.

Botão A: Ativa ou desativa o controle de brilho dos LEDs Vermelho e Azul (PWM).

Debouncing:

Implementado via software para evitar leituras incorretas dos botões.

## Como executar 

Clone o repositório do projeto para o seu ambiente local e abri-lo no VS Code.

Antes de compilar o código, é necessário garantir que todas as extensões e ferramentas estão devidamente instaladas.

Instalar o Pico SDK e extensão Raspberry Pi Pico: Siga as instruções do Pico SDK para configurar o ambiente de desenvolvimento no VS Code e instale a extensão Raspberry Pi Pico.

Com o ambiente devidamente configurado, abra o código no VS Code e compile o projeto a partir do menu de compilação da extensão Raspberry Pi Pico.

Carregar o código na placa BitDogLab(Para ter melhores resultados, utilizar somente com a BitDogLab, ou utilizar os componentes nescessários conectados as GPIOs corretas indicadas em tópicos anteriores).

A partir dai pode aproveitar e testar as funcionalidades.

## Vídeo de Demonstração

[DRIVE](https://drive.google.com/drive/folders/1pYbtjSt5DTyY-uvxDiIyF3RlkeF0-SQx?usp=sharing)

## Projeto por: Alex Rodrigo Porto Barbosa
