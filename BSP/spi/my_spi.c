#include "my_spi.h"
#include "string.h"

#define GPIO_OUTPUT_PIN_SEL (1ULL << CS) | (1ULL << DC) | (1ULL << RST)
#define GPIO_INPUT_PIN_SEL (1ULL << BUSY)

spi_device_handle_t spi2_handle;

/*gpio init*/
void gpio_init(void)
{
    gpio_config_t output_conf;
    output_conf.intr_type = GPIO_INTR_DISABLE;
    output_conf.mode = GPIO_MODE_OUTPUT;
    output_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    output_conf.pull_down_en = 0;
    output_conf.pull_up_en = 1;
    gpio_config(&output_conf);

    gpio_config_t input_conf;
    input_conf.intr_type = GPIO_INTR_DISABLE;
    input_conf.mode = GPIO_MODE_INPUT;
    input_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    gpio_config(&input_conf);
}

/*spi2 init*/
void spi_init(void)
{
    spi_bus_config_t spi_config = {
        .mosi_io_num = MOSI,
        .miso_io_num = -1,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .sclk_io_num = SCK,
        .max_transfer_sz = 64,
    };

    /*bus初始化*/
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_config, SPI_DMA_DISABLED)); // 不开DMA,最大传输64个字节

    /*接口初始化*/
    spi_device_interface_config_t dev_config = {
        .clock_speed_hz = 1000000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 7,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &dev_config, &spi2_handle));
}

/*type参数是1 发送 data,0 发送 cmd
 *
 */
void spi_write(int type, uint8_t Tx_data)
{
    uint8_t send_data[] = {Tx_data};
    spi_transaction_t trans_desc;
    memset(&trans_desc, 0, sizeof(trans_desc));
    trans_desc.length = 8;                   // 设定要发送的位数(bit)
    trans_desc.user = (void *)1;
    trans_desc.tx_buffer = send_data;

    gpio_set_level(CS, 0);
    if (type == cmd)
    {
        gpio_set_level(DC, cmd);
    }
    else
    {
        gpio_set_level(DC, data);
    }
    // ESP_ERROR_CHECK(spi_device_polling_transmit(spi2_handle, &trans_desc));
    ESP_ERROR_CHECK(spi_device_transmit(spi2_handle, &trans_desc));
    gpio_set_level(CS, 1);
}