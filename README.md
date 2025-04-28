# Estação de Monitoramento Interativo  

## Descrição  
Este projeto simula uma estação de monitoramento com sensores e atuadores, utilizando um sistema de dois núcleos que se comunicam entre si. Ele lê estados de um joystick e atualiza LEDs e um buzzer de acordo com a atividade detectada.  

## Aluno  
**Nome:** Wasley dos Santos Silva  
**Turma:** ALFA embarcatech 
**Matrícula:** 20251RSE.MTC0037  

## Componentes  
- **Placa:** BitDogLab  
- **Sensores e Atuadores:** Joystick, LEDs e Buzzer da placa BitDogLab.  

## Funcionamento  
- **Estados do Joystick:**  
  - **Atividade Neutra:** Joystick sem uso (LED Branco aceso)  
  - **Atividade Alta:** Joystick para cima (LED Vermelho aceso)  
  - **Atividade Moderada:** Joystick para os lados (LED Azul aceso)  
  - **Atividade Baixa:** Joystick para baixo (LED Verde aceso)  

- O estado do joystick é atualizado no console a cada 2 segundos, acompanhado de um bipe sonoro.  

## Código  
O código é escrito em C e utilizado para o controle dos dispositivos: 
