#include "Epaper_func.h"
#include "my_spi.h"

void delay_ms(uint16_t time_ms)
{
    vTaskDelay(pdMS_TO_TICKS(time_ms));
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void Epaper_Reset(void)
{
    gpio_set_level(RST, 1);
    delay_ms(20);
    gpio_set_level(RST, 0);
    delay_ms(2);
    gpio_set_level(RST, 1);
    delay_ms(20);
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void Epaper_ReadBusy(void)
{
    while (1)
    {
        if (Read_BUSY_Status == 0)
            break;
        delay_ms(10);
    }
    delay_ms(10);
}

/******************************************************************************
function :	Setting the display window
parameter:
    Xstart : X-axis starting position
    Ystart : Y-axis starting position
    Xend : End position of X-axis
    Yend : End position of Y-axis
******************************************************************************/
static void Epaper_SetWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    spi_write(cmd, 0x44);
    spi_write(data, (Xstart >> 3) & 0xFF);
    spi_write(data, (Xend >> 3) & 0xFF);

    spi_write(cmd, 0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    spi_write(data, Ystart & 0xFF);
    spi_write(data, (Ystart >> 8) & 0xFF);
    spi_write(data, Yend & 0xFF);
    spi_write(data, (Yend >> 8) & 0xFF);
}

/******************************************************************************
function :	Set Cursor
parameter:
    Xstart : X-axis starting position
    Ystart : Y-axis starting position
******************************************************************************/
static void Epaper_SetCursor(uint16_t Xstart, uint16_t Ystart)
{
    spi_write(cmd, 0x4E); // SET_RAM_X_ADDRESS_COUNTER
    spi_write(data, Xstart & 0xFF);

    spi_write(cmd, 0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    spi_write(data, Ystart & 0xFF);
    spi_write(data, (Ystart >> 8) & 0xFF);
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void Epaper_TurnOnDisplay(void)
{
    spi_write(cmd, 0x22); // Display Update Control
    spi_write(data, 0xf7);
    spi_write(cmd, 0x20); // Activate Display Update Sequence
    Epaper_ReadBusy();
}

static void Epaper_TurnOnDisplay_Fast(void)
{
    spi_write(cmd,0x22); // Display Update Control
    spi_write(data,0xc7);    // fast:0x0c, quality:0x0f, 0xcf
    spi_write(cmd,0x20); // Activate Display Update Sequence
    Epaper_ReadBusy();
}

static void Epaper_TurnOnDisplay_Partial(void)
{
    spi_write(cmd,0x22); // Display Update Control
    spi_write(data,0xff);    // fast:0x0c, quality:0x0f, 0xcf
    spi_write(cmd,0x20); // Activate Display Update Sequence
    Epaper_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void Epaper_Init(void)
{
    Epaper_Reset();

    Epaper_ReadBusy();
    spi_write(cmd,0x12); // SWRESET
    Epaper_ReadBusy();

    spi_write(cmd,0x01); // Driver output control
    spi_write(data,0xF9);
    spi_write(data,0x00);
    spi_write(data,0x00);

    spi_write(cmd,0x11); // data entry mode
    spi_write(data,0x03);

    Epaper_SetWindows(0, 0, WIDTH - 1, HEIGHT - 1);
    Epaper_SetCursor(0, 0);

    spi_write(cmd,0x3C); // BorderWavefrom
    spi_write(data,0x05);

    spi_write(cmd,0x21); //  Display update control
    spi_write(data,0x00);
    spi_write(data,0x80);

    spi_write(cmd,0x18); // Read built-in temperature sensor
    spi_write(data,0x80);
    Epaper_ReadBusy();
}

void Epaper_Init_Fast(void)
{
    Epaper_Reset();

    spi_write(cmd,0x12); // SWRESET
    Epaper_ReadBusy();

    spi_write(cmd,0x18); // Read built-in temperature sensor
    spi_write(data,0x80);

    spi_write(cmd,0x11); // data entry mode
    spi_write(data,0x03);

    Epaper_SetWindows(0, 0, WIDTH - 1, HEIGHT - 1);
    Epaper_SetCursor(0, 0);

    spi_write(cmd,0x22); // Load temperature value
    spi_write(data,0xB1);
    spi_write(cmd,0x20);
    Epaper_ReadBusy();

    spi_write(cmd,0x1A); // Write to temperature register
    spi_write(data,0x64);
    spi_write(data,0x00);

    spi_write(cmd,0x22); // Load temperature value
    spi_write(data,0x91);
    spi_write(cmd,0x20);
    Epaper_ReadBusy();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void Epaper_Clear(void)
{
    uint16_t Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    spi_write(cmd,0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,0XFF);
        }
    }

    Epaper_TurnOnDisplay();
}

void Epaper_Clear_Black(void)
{
    uint16_t Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    spi_write(cmd,0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,0X00);
        }
    }

    Epaper_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
    Image : Image data
******************************************************************************/
void Epaper_Display(uint8_t *Image)
{
    uint16_t Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    spi_write(cmd,0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,Image[i + j * Width]);
        }
    }

    Epaper_TurnOnDisplay();
}

void Epaper_Display_Fast(uint8_t *Image)
{
    uint16_t Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    spi_write(cmd,0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,Image[i + j * Width]);
        }
    }

    Epaper_TurnOnDisplay_Fast();
}

/******************************************************************************
function :	Refresh a base image
parameter:
    Image : Image data
******************************************************************************/
void Epaper_Display_Base(uint8_t *Image)
{
    uint16_t Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    spi_write(cmd,0x24); // Write Black and White image to RAM
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,Image[i + j * Width]);
        }
    }
    spi_write(cmd,0x26); // Write Black and White image to RAM
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,Image[i + j * Width]);
        }
    }
    Epaper_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and partial refresh
parameter:
    Image : Image data
******************************************************************************/
void Epaper_Display_Partial(uint8_t *Image)
{
    uint16_t Width, Height;
    Width = (WIDTH % 8 == 0) ? (WIDTH / 8) : (WIDTH / 8 + 1);
    Height = HEIGHT;

    // Reset
    gpio_set_level(RST, 0);
    delay_ms(2);
    gpio_set_level(RST, 1);

    spi_write(cmd,0x3C); // BorderWavefrom
    spi_write(data,0x80);

    spi_write(cmd,0x01); // Driver output control
    spi_write(data,0xF9);
    spi_write(data,0x00);
    spi_write(data,0x00);

    spi_write(cmd,0x11); // data entry mode
    spi_write(data,0x03);

    Epaper_SetWindows(0, 0, WIDTH - 1, HEIGHT - 1);
    Epaper_SetCursor(0, 0);

    spi_write(cmd,0x24); // Write Black and White image to RAM
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            spi_write(data,Image[i + j * Width]);
        }
    }
    Epaper_TurnOnDisplay_Partial();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void Epaper_Sleep(void)
{
    spi_write(cmd,0x10); // enter deep sleep
    spi_write(data,0x01);
    delay_ms(100);
}
