cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(chuni-pico)

pico_sdk_init()

add_executable(chu-pico
    main.c
    mpr121.c
    vl53l0x.c
    config.c
    save.c
    rgb.c
)

target_link_libraries(chu-pico
        pico_multicore pico_stdlib hardware_pio hardware_pwm hardware_flash
        hardware_adc hardware_i2c hardware_watchdog
        
)

pico_enable_stdio_usb(chu-pico 1)
pico_enable_stdio_uart(chu-pico 0)

pico_add_extra_outputs(chu-pico)