/* Waveshare Epaper Example */

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "epd.h"

static void
epd_demo( epd_t *epd )
{
    uint8_t row[ EPD_WIDTH / 2 ];

    epdBegin( epd );

#define BLOCK (EPD_HEIGHT/7)
    for (int i=0; i<EPD_HEIGHT; i++) {
        // coloured rows
        uint8_t v;
        if      ( i < ( BLOCK*1) ) v = 0x00;
        else if ( i < ( BLOCK*2) ) v = 0x11;
        else if ( i < ( BLOCK*3) ) v = 0x22;
        else if ( i < ( BLOCK*4) ) v = 0x33;
        else if ( i < ( BLOCK*5) ) v = 0x44;
        else if ( i < ( BLOCK*6) ) v = 0x55;
        else if ( i < ( BLOCK*7) ) v = 0x66;
        else v = 0x00; // shouldn't happen, shut the warning up.

        memset( row, v, EPD_WIDTH / 2 );
        epdPush( epd, row, EPD_WIDTH / 2 );
    }

    epdEnd( epd );
}

void app_main(void)
{
    printf("Hello world!\n");

    epd_t epaper = {
        .pins = {
            .BUSY = 23,
            .RST = 18,
            .DC = 17,
            .CS = 15,
            .MOSI = 13,
            .SCK = 14,
        }
    };

    ESP_ERROR_CHECK( createEpd(&epaper) );
    printf("Opened Epaper\n");
    initEpd(&epaper);
    epdClear(&epaper);
    epd_demo(&epaper);

    epdSleep(&epaper);

    printf("Finished!\n");
    for (;; ) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
