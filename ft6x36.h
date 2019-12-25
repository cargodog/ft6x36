#ifndef __FT6X36_H__
#define __FT6X36_H__

#include <stdint.h>
#include <stdbool.h>

#define FT6X36_DEV_ADDRESS      (0x38U)
#define FT6X36_MAX_TOUCH_POINTS 2

enum ft6x36TouchEvent {
    FT6X36_EVT_PRESS    = 0x00,
    FT6X36_EVT_RELEASE  = 0x01,
    FT6X36_EVT_CONTACT  = 0x02,
    FT6X36_EVT_NONE     = 0x03
};

typedef bool (*FT6X36ReadPin_t)(void);
typedef void (*FT6X36Reset_t)(void);
typedef void (*FT6X36TransferI2C_t)(uint32_t addr,
                                    uint32_t reg,
                                    void * data,
                                    int len );

typedef struct {
    FT6X36ReadPin_t     fnReadIntPin;   // Read the GPIO level of the INT pin
    FT6X36Reset_t       fnResetChip;    // Reset the FT6x36 device
    FT6X36TransferI2C_t fnSerialRead;   // I2C read from the FT6x36 device
    FT6X36TransferI2C_t fnSerialWrite;  // I2C write to the FT6x36 device
} FT6X36Config_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t event;
    uint8_t touchID;
    uint8_t weight;
    uint8_t area;
} FT6X36Touchpoint_t;

void       ft6326_init(const FT6X36Config_t * config);
bool    ft6x36_touched(void);
int ft6x36_get_touches(FT6X36Touchpoint_t tp[FT6X36_MAX_TOUCH_POINTS]);

#endif /* __FT6X36_H__ */