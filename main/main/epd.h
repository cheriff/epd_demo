#ifndef __EPD_H__
#define __EPD_H__

#include <stdint.h>
#include <stddef.h>

#define EPD_BLACK  (0x000000) //   0000  BGR
#define EPD_WHITE  (0xffffff) //   0001
#define EPD_GREEN  (0x00ff00) //   0010
#define EPD_BLUE   (0xff0000) //   0011
#define EPD_RED    (0x0000ff) //   0100
#define EPD_YELLOW (0x00ffff) //   0101
#define EPD_ORANGE (0x0080ff) //   0110

extern const int EPD_WIDTH; // 600;
extern const int EPD_HEIGHT; // 448;

typedef struct {
    int RST;
    int DC;
    int BUSY;
    int CS;
    int MOSI;
    int SCK;
} epd_pins_t;

struct epd;
typedef struct epd epd_t;

typedef struct {
    int (*pin_set)( epd_t*, int pin, int val );
    int (*pin_get)( epd_t*, int pin );
    int (*spi_write)( epd_t*, const uint8_t *, size_t sz);
    void (*sleep_ms)( epd_t*, int ms );
} epd_hw_callbacks_t;


struct epd {
    epd_pins_t pins;
    epd_hw_callbacks_t callbacks;

    /* priv might hold something board-specific,
     * like: `spi_inst_t *spi;`
     */
    void *priv;
};

/* Platform-provided helper to fully populate the epd_t:
 * set pins, callbacks, priv and also any hw init as needed
 */
int createEpd( epd_t *epd);

/* All following funcitons are board-generic via the callbacks */
int initEpd( epd_t *epd );

/* Drawing:
 * Begin()
 * Push exactly 600*448/2 bytes
 * End()
 */
void epdBegin( epd_t *epd );
void epdPush( epd_t *epd, const uint8_t *data, size_t len );
void epdEnd( epd_t *epd );

/* Clear is just drawing white 0x44 pixels */
void epdClear( epd_t *epd );

void epdSleep( epd_t *epd );



#endif // __EPD_H__
