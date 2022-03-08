#include "epd.h"

#include <stdio.h>
#include <string.h>

const int EPD_WIDTH       = 600;
const int EPD_HEIGHT      = 448;

#define gpio_put( _pin, _val ) epd->callbacks.pin_set( epd, epd->pins._pin, _val )
#define gpio_get( _pin ) epd->callbacks.pin_get( epd, epd->pins._pin )
#define sleep_ms( _ms ) epd->callbacks.sleep_ms( epd, _ms )
#define spi_write_blocking( _buff, _sz ) epd->callbacks.spi_write( epd, _buff, _sz )

static void
resetEpd( epd_t *epd )
{
    gpio_put( RST, 1);
    sleep_ms(600);
    gpio_put( RST, 0);
    sleep_ms(2);
    gpio_put( RST, 1);
    sleep_ms(200);
}


static void
epdSendCommand( epd_t *epd, const uint8_t cmd )
{
    gpio_put( DC, 0);
    // it is up to the platform implementation (sw or hw)
    // to manage ChipSelect
    spi_write_blocking( &cmd, 1 );
}

static void
epdSendData( epd_t *epd, const uint8_t *src, size_t sz )
{
    gpio_put( DC, 1);
    // again, CS managed in platform layer
    spi_write_blocking( src, sz );
}

static void
epdSendData1( epd_t *epd, const uint8_t d )
{
    epdSendData( epd, &d, 1 );
}

static void
epdReadBusyHigh( epd_t *epd )
{
    printf("Waiting BusyHigh ..\n" );
    while( gpio_get( BUSY ) == 0) {
        sleep_ms(100);
    }
    printf("Waiting Done\n" );
}

static void
epdReadBusyLow( epd_t *epd )
{
    printf("Waiting BusyLow..\n" );
    while( gpio_get( BUSY ) == 1) {
        sleep_ms(100);
    }
    printf("Waiting Done\n" );
}

int
initEpd( epd_t *epd )
{
    /* reset process */
    resetEpd( epd );
    epdReadBusyHigh( epd );
    epdSendCommand( epd, 0x00);
    epdSendData1( epd, 0xEF);
    epdSendData1( epd, 0x08);
    epdSendCommand( epd, 0x01);
    epdSendData1( epd, 0x37);
    epdSendData1( epd, 0x00);
    epdSendData1( epd, 0x23);
    epdSendData1( epd, 0x23);
    epdSendCommand( epd, 0x03);
    epdSendData1( epd, 0x00);
    epdSendCommand( epd, 0x06);
    epdSendData1( epd, 0xC7);
    epdSendData1( epd, 0xC7);
    epdSendData1( epd, 0x1D);
    epdSendCommand( epd, 0x30);
    epdSendData1( epd, 0x3C);
    epdSendCommand( epd, 0x40);
    epdSendData1( epd, 0x00);
    epdSendCommand( epd, 0x50);
    epdSendData1( epd, 0x37);
    epdSendCommand( epd, 0x60);
    epdSendData1( epd, 0x22);
    epdSendCommand( epd, 0x61);
    epdSendData1( epd, 0x02);
    epdSendData1( epd, 0x58);
    epdSendData1( epd, 0x01);
    epdSendData1( epd, 0xC0);
    epdSendCommand( epd, 0xE3);
    epdSendData1( epd, 0xAA);

    sleep_ms(100);
    epdSendCommand( epd, 0x50);
    epdSendData1( epd, 0x37);

    return 0;
}

void
epdBegin( epd_t *epd )
{
    epdSendCommand( epd, 0x61 ); //Set Resolution setting
    epdSendData1( epd, 0x02);
    epdSendData1( epd, 0x58);
    epdSendData1( epd, 0x01);
    epdSendData1( epd, 0xC0);
    epdSendCommand( epd,0x10);
}

void
epdPush( epd_t *epd, const uint8_t *data, size_t len )
{
    epdSendData( epd, data, len );
}

void epdClear( epd_t *epd )
{
    uint8_t row[ EPD_WIDTH / 2 ];

    epdBegin( epd );

#define BLOCK (EPD_HEIGHT/7)
    for (int i=0; i<EPD_HEIGHT; i++) {
        uint8_t v;
        if      ( i < ( BLOCK*1) ) v = 0x00;
        else if ( i < ( BLOCK*2) ) v = 0x11;
        else if ( i < ( BLOCK*3) ) v = 0x22;
        else if ( i < ( BLOCK*4) ) v = 0x33;
        else if ( i < ( BLOCK*5) ) v = 0x44;
        else if ( i < ( BLOCK*6) ) v = 0x55;
        else if ( i < ( BLOCK*7) ) v = 0x66;

        v = 0x11;;
        memset( row, v, EPD_WIDTH / 2 );
        epdSendData( epd, row, EPD_WIDTH / 2 );
    }

    epdEnd( epd );
}

void epdEnd( epd_t *epd )
{
    epdSendCommand( epd, 0x04); //0x04
    epdReadBusyHigh( epd );
    epdSendCommand( epd,0x12); //#0x12
    epdReadBusyHigh( epd );
    epdSendCommand( epd,0x02);//  #0x02
    epdReadBusyLow( epd );
    sleep_ms(500);
}

void epdSleep( epd_t *epd )
{
    sleep_ms(500);
    epdSendCommand( epd, 0x07); //DEEP_SLEEP
    epdSendData1( epd, 0XA5);
    gpio_put( RST, 0);
}
