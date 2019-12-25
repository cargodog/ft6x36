#include "ft6x36.h"
#include <assert.h>


static FT6X36Config_t cfg = {0};

#define read_int_pin() cfg.fnReadIntPin() // INT pin is active low
#define reset_chip() cfg.fnResetChip()
#define serial_read(reg, data, len)\
    cfg.fnSerialRead(FT6X36_DEV_ADDRESS, reg, data, len)
#define serial_write(reg, data, len)\
    cfg.fnSerialWrite(FT6X36_DEV_ADDRESS, reg, data, len)

static inline uint8_t serial_read_single(uint32_t reg)
{
    uint8_t val;
    serial_read(reg, &val, 1);
    return val;
}

static inline void serial_write_single(uint32_t reg, uint8_t val)
{
    return serial_write(reg, &val, 1);
}

static void check_config(void)
{
    assert(cfg.fnReadIntPin);
    assert(cfg.fnResetChip);
    assert(cfg.fnSerialRead);
    assert(cfg.fnSerialWrite);
}


void ft6326_init(const FT6X36Config_t * config)
{
    assert(config);

    // Save the config
    cfg = *config;

    // Check the configuration
    check_config();
    
    // Reset the chip
    reset_chip();

    // Make sure chip vendor ID is supported
    assert(0x11 == serial_read_single(0xA8));
    
    // Make sure chip release ID is supported
    assert(0x01 == serial_read_single(0xAF));

    // Put the chip in "working" mode of operation
    serial_write_single(0x00, 0);

    // Set the capture mode to "interrupt-polling" mode
    serial_write_single(0xA4, 0);
}

bool ft6x36_touched(void)
{
    check_config();
    return !read_int_pin();
}

int ft6x36_get_touches(FT6X36Touchpoint_t tp[FT6X36_MAX_TOUCH_POINTS])
{
    static const int TOUCH_DATA_SIZE = 6; // 6 bytes per touch point
    uint8_t data[FT6X36_MAX_TOUCH_POINTS * TOUCH_DATA_SIZE];
    check_config();
    
    // Read status register and see if touches exist
    uint8_t touchCount = serial_read_single(0x02) & 0x0f;
    if(touchCount > FT6X36_MAX_TOUCH_POINTS)
        touchCount = 0; // Bug: At startup, reports 15 touches, instead of 0

    // Read out touch points
    if(touchCount)
        serial_read(0x03, data, TOUCH_DATA_SIZE * touchCount);

    // Map touch data to FT6X36Touchpoint_t type
    for(int i = 0; i < touchCount; i++) {
        uint8_t * tpData = &data[i * TOUCH_DATA_SIZE];
        tp[i].x         = ((tpData[0] & 0x0F) << 8) + tpData[1];
        tp[i].y         = ((tpData[2] & 0x0F) << 8) + tpData[3];
        tp[i].event     = (tpData[0] & 0xC0) >> 6;
        tp[i].touchID   = (tpData[2] & 0xF0) >> 4;
        tp[i].weight    = (tpData[4] & 0xFF) >> 0;
        tp[i].area      = (tpData[5] & 0xF0) >> 4;
    }

    return touchCount;
}