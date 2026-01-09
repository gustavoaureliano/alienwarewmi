#ifndef LIB_ALIENWARE_WMI_H
#define LIB_ALIENWARE_WMI_H

#include <stdint.h>

typedef enum {
	booting,
	running,
	suspend
} LightingControlState;

typedef enum {
	head,  // zone00 - power button led
	left,  // zone01 - actual leds
	right, // zone02 - not really used on inspiron 5675
} Zone;

typedef struct {
	Zone zone;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} RGBZoneColor;

void check_platform_exists(void);
void check_rgb_zones_exists(void);
void set_rgb_zone_color(RGBZoneColor color);
RGBZoneColor get_rgb_zone_color(Zone zone);
void set_lighting_control_state(LightingControlState state);

#endif // LIB_ALIENWARE_WMI_H
