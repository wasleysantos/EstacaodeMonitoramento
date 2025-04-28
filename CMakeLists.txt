# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(EstacaodeMonitoramento C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(EstacaodeMonitoramento 
EstacaodeMonitoramento.c
)

# Habilitar saída via UART e USB
pico_enable_stdio_uart(EstacaodeMonitoramento 1)
pico_enable_stdio_usb(EstacaodeMonitoramento 1)

# Adicionar bibliotecas padrão (stdlib, multicore, adc, gpio, irq)
target_link_libraries(EstacaodeMonitoramento
    pico_stdlib
    hardware_adc
    hardware_gpio
    hardware_irq
    pico_multicore
)

# Incluir diretórios locais
target_include_directories(EstacaodeMonitoramento PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Gerar arquivos extras (como UF2 para gravação)
pico_add_extra_outputs(EstacaodeMonitoramento)
