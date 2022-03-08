
#include "driver/spi_master.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "epd.h"


static esp_err_t
easy_gpio_setup( int pin, gpio_mode_t mode )
{
    esp_err_t result;
    const gpio_config_t cfg = {
        .intr_type = GPIO_PIN_INTR_DISABLE,
        .mode = mode,
        .pin_bit_mask = (1ULL<<pin),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    result = gpio_config( &cfg );
    if (result != ESP_OK)
        printf("Warning: Failed to config pin %d :: %d\n",
            pin, result);
    printf("Configured GPIO %d - %d\n", pin, result);
    return result;
}

static inline void
delay_ms(int ms){ vTaskDelay(ms / portTICK_PERIOD_MS); }

static inline int
spi_writebyte(spi_device_handle_t spi, uint8_t byte)
{
    struct spi_transaction_t tx = {
        .flags =  SPI_TRANS_USE_TXDATA,
        .rxlength = 0,
        .length = 8,
        .tx_data = { byte },
    };
    int res = spi_device_polling_transmit(spi, &tx);
    return res;
}

int
spi_writebytes(spi_device_handle_t spi, const uint8_t *bytes, size_t len){

    while (len > 0 ) {
        int burst = len;
        if (burst>64) burst = 64;
        struct spi_transaction_t tx = {
            .flags =  0, //SPI_TRANS_USE_TXDATA,
            .rxlength = 0,
            .length = 8*burst,
            .tx_buffer = bytes,
        };
        spi_device_polling_transmit(spi, &tx);

        bytes+=burst;
        len-=burst;
    }
    return 0;
}

static void
esp32_sleep_ms( epd_t* epd, int ms ) {
    delay_ms(ms);
}

static int
esp32_spi_write( epd_t* epd, const uint8_t *buff, size_t sz )
{
    if( sz== 1) {
        return spi_writebyte(epd->priv, buff[0]);
    } else {
        return spi_writebytes(epd->priv, buff, sz);
    }
}

int esp32_pin_set( epd_t *epaper, int pin, int val )
{
    return gpio_set_level(pin, val);
}
int esp32_pin_get( epd_t *epaper, int pin)
{
    return gpio_get_level(pin);
}


int
createEpd( epd_t *epaper)
{
    assert(epaper != NULL);

    epaper->callbacks.pin_set = esp32_pin_set;
    epaper->callbacks.pin_get = esp32_pin_get;
    epaper->callbacks.spi_write = esp32_spi_write;
    epaper->callbacks.sleep_ms = esp32_sleep_ms;

    // GPIO setup
    ESP_ERROR_CHECK( easy_gpio_setup( epaper->pins.BUSY,  GPIO_MODE_INPUT ) );
    ESP_ERROR_CHECK( easy_gpio_setup( epaper->pins.RST, GPIO_MODE_OUTPUT ) );
    ESP_ERROR_CHECK( easy_gpio_setup( epaper->pins.DC,    GPIO_MODE_OUTPUT ) );
    /* others are managed by SPI controller? */

    // SPI BUS setup
    spi_bus_config_t buscfg = {
        .miso_io_num= -1, // no reading from paper
        .mosi_io_num=epaper->pins.MOSI,
        .sclk_io_num=epaper->pins.SCK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz= 0, // sensible default?
    };
    ESP_ERROR_CHECK( spi_bus_initialize(SPI2_HOST, &buscfg, 0) );

    // SPI Device setup
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
        .mode=0,                               //SPI mode 0
        .spics_io_num=epaper->pins.CS,
        .queue_size=2,
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
    };
    ESP_ERROR_CHECK( spi_bus_add_device(SPI2_HOST, &devcfg,
                (spi_device_handle_t*)&epaper->priv) );

    printf("epaper open OK\n");
    return ESP_OK ;
}





