#include <stdio.h>
#include <stdint.h> // UTILIZAR O UINT
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h" // CONTROLE DA CAMERA
#include "esp_system.h" // FUNÇOES DO ESP
#include "esp_log.h"
#include "driver/gpio.h"

#define LED_PIN 13
#define BUZZER_GPIO 12

// CONFIGURAÇAO DOS PINOS DA CAMERA
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27
#define CAM_PIN_Y9      35
#define CAM_PIN_Y8      34
#define CAM_PIN_Y7      39
#define CAM_PIN_Y6      36
#define CAM_PIN_Y5      21
#define CAM_PIN_Y4      19
#define CAM_PIN_Y3      18
#define CAM_PIN_Y2       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22


//configuracao da camera
camera_config_t config = {
    pin_pwdn = CAM_PIN_PWDN,
    pin_reset = CAM_PIN_RESET,
    pin_xclk = CAM_PIN_XCLK,
    pin_sccb_sda = CAM_PIN_SIOD,
    pin_sccb_scl = CAM_PIN_SIOC,
    pin_d7 = CAM_PIN_Y9,
    pin_d6 = CAM_PIN_Y8,
    pin_d5 = CAM_PIN_Y7,
    pin_d4 = CAM_PIN_Y6,
    pin_d3 = CAM_PIN_Y5,
    pin_d2 = CAM_PIN_Y4,
    pin_d1 = CAM_PIN_Y3,
    pin_d0 = CAM_PIN_Y2,
    pin_vsync = CAM_PIN_VSYNC,
    pin_href = CAM_PIN_HREF,
    pin_pclk = CAM_PIN_PCLK,

    xclk_freq_hz = 20000000,
    pixel_format = PIXFORMAT_GRAYSCALE,
    frame_size = FRAMESIZE_QQVGA,
    jpeg_quality = 12,
    fb_count = 1
};

void app_main(void)
{
gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);
}
 while (1)
 {

 }