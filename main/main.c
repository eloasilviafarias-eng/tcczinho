#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <stdlib.h>
#include "esp_psram.h"

#define LIMIAR_PIXELS 20
#define LIMIAR_MOVIMENTO 150
#define LIMIAR_LUZ 80
#define FRAMES_CONFIRMACAO 4

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

uint8_t *frame_anterior = NULL;
int contador_movimento = 0;

// CONFIG CAMERA
camera_config_t config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,
    .pin_d7 = CAM_PIN_Y9,
    .pin_d6 = CAM_PIN_Y8,
    .pin_d5 = CAM_PIN_Y7,
    .pin_d4 = CAM_PIN_Y6,
    .pin_d3 = CAM_PIN_Y5,
    .pin_d2 = CAM_PIN_Y4,
    .pin_d1 = CAM_PIN_Y3,
    .pin_d0 = CAM_PIN_Y2,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    .xclk_freq_hz = 20000000,
    .pixel_format = PIXFORMAT_GRAYSCALE,
    .frame_size = FRAMESIZE_QQVGA,
    .jpeg_quality = 12,
    .fb_count = 1
};

void app_main(void)
{
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(LED_PIN, 0);
    gpio_set_level(BUZZER_GPIO, 0);
 
    // memoria
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    if (esp_psram_is_initialized()) {
        config.fb_count = 2;
    }

    // iniciar camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
       ESP_LOGE("CAMERA", "Erro: %s", esp_err_to_name(err));
        return;
    }

    while (1)
    {
        camera_fb_t *fb = esp_camera_fb_get();

        if (!fb) {
            printf("Erro na captura de imagem\n");
        }   
        else {

            if (frame_anterior == NULL) {
                frame_anterior = malloc(fb->len);

                if (frame_anterior == NULL) {
                    printf("Erro de memoria\n");
                    esp_camera_fb_return(fb);
                    continue;
                }

                memcpy(frame_anterior, fb->buf, fb->len);
                esp_camera_fb_return(fb);
                continue;
            }

            int diferenca = 0;

            int largura = fb->width;
            int altura = fb->height;

            // analisa região central
            for (int y = altura/3; y < (altura*2)/3; y++) {
                for (int x = largura/4; x < (largura*3)/4; x += 5) {

                    int i = y * largura + x;
                    if (i >= fb->len) continue;

                    int diff = fb->buf[i] - frame_anterior[i];
                    if (diff < 0) diff = -diff;

                    if (diff > LIMIAR_PIXELS) {
                        diferenca++;
                    }
                }
            }

            printf("Diferenca: %d\n", diferenca);

            // deteccao
            if (diferenca > LIMIAR_MOVIMENTO && diferenca < 2000) {
                contador_movimento++;
            } else {
                contador_movimento = 0;
            }

            if (contador_movimento > FRAMES_CONFIRMACAO) {
                printf("PEDESTRE DETECTADO\n");

                gpio_set_level(BUZZER_GPIO, 1);
                vTaskDelay(200 / portTICK_PERIOD_MS);
                gpio_set_level(BUZZER_GPIO, 0);
            } else {
                gpio_set_level(BUZZER_GPIO, 0);
            }

            // atualiza frame
            memcpy(frame_anterior, fb->buf, fb->len);

            // luminosidade
            long soma = 0;

            for (int i = 0; i < fb->len; i++) {
                soma += fb->buf[i];
            }

            int media = soma / fb->len;

            printf("Luminosidade: %d\n", media);

            if (media < LIMIAR_LUZ) {
                gpio_set_level(LED_PIN, 1);
            } else {
                gpio_set_level(LED_PIN, 0);
            }

            esp_camera_fb_return(fb);
        }

        vTaskDelay(150 / portTICK_PERIOD_MS);
    }
}