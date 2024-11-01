#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "my_spi.h"
#include "Epaper_func.h"
#include "GUI_Paint.h"
#include "ImageData.h"

void epaper_driver_init(void) // epaper的io初始化
{
    gpio_init();
    spi_init();

    /*Epaper init*/
}

void app_main(void)
{
    epaper_driver_init();
    Epaper_Init();
    Epaper_Clear();

    uint8_t *BlackImage;
    uint16_t Imagesize = ((WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1)) * HEIGHT;
    BlackImage = (uint8_t *)malloc(Imagesize);
    Paint_NewImage(BlackImage, WIDTH, HEIGHT, 270, WHITE);
#if 0 // display 数字
    Epaper_Init_Fast();
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    // Paint_DrawBitMap(gImage_2in13_2);
    // Paint_ClearWindows(0,0,64,64,BLACK);//刷黑一块区域
    int number = 101;

    Paint_DrawNum(0, 0, number, &Font12, BLACK, WHITE);
    Paint_DrawNum(0, 12, number + 1, &Font12, BLACK, WHITE); //x范围0~250,y范围0~128,rotate = 270
    delay_ms(2000);
    Epaper_Display_Fast(BlackImage);

#endif

#if 1 // display QR_code图片,图片尺寸必须250*122
    Epaper_Init_Fast();
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(QR_code);
    Epaper_Display_Fast(BlackImage);

#endif

    Epaper_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2s.
    }
}