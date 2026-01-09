#include <stdio.h>
#include <string.h>

#include "libalienwarewmi.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("usage: inspiron-cli on/off\n");
		return 0;
	}
	check_platform_exists();
	check_rgb_zones_exists();
	Zone default_zone = left;
	RGBZoneColor color_on = {default_zone, 0, 0, 15};
	RGBZoneColor color_off = {default_zone, 0, 0, 0};
	if (strcmp(argv[1], "get") == 0) {
		RGBZoneColor zonecolor = get_rgb_zone_color(default_zone);
		printf("red: %d, green: %d, blue: %d\n", zonecolor.red, zonecolor.green, zonecolor.blue);
		return 0;
	} else if ((strcmp(argv[1], "set")) == 0) {
		if (argc == 2) {
			printf("invalid option! choose either 'set on' or 'set off'\n");
			return 1;
		}
		if (strcmp(argv[2], "on") == 0) {
			printf("turn on\n");
			set_rgb_zone_color(color_on);
			return 0;
		} else if (strcmp(argv[2], "off") == 0) {
			printf("turn off\n");
			set_rgb_zone_color(color_off);
			return 0;
		} else {
			printf("invalid option! choose either 'on' or 'off'\n");
			return 1;
		}
	}
	printf("invalid option! choose either 'get' or 'set'\n");
	return 1;
}
