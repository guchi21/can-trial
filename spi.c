#include "spi.h"

/* SPI baudrate definition in Hz */
#define SPI_CLOCK 1000000U

void spi_write(uint8_t value)
{
    spi_write_blocking(spi0, &value, 1);
}

uint8_t spi_read(void)
{
    uint8_t buf[1];

    spi_read_blocking(spi0, 0, buf, 1);

    return buf[0];
}

void spi_configure(void)
{
    /*-----------------------------------------------------------*/
    /* Baudrate: 10MHz, RX: 4pin, CSn: 5pin, SCK: 6pin, TX: 7pin */
    /*-----------------------------------------------------------*/

    spi_init(spi0, SPI_CLOCK);

    gpio_set_function(PIN_SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI_SCK, GPIO_FUNC_SPI);

    gpio_init(PIN_SPI_CSN);
    gpio_set_dir(PIN_SPI_CSN, GPIO_OUT);
    gpio_put(PIN_SPI_CSN, HIGH);
}
