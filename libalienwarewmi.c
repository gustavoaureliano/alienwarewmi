#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

#include "libalienwarewmi.h"

#define PLATFORM_DIR "/sys/devices/platform/alienware-wmi"
#define RGB_ZONES_DIR PLATFORM_DIR "/rgb_zones"
#define LIGHTING_CONTROL_STATE_PATH RGB_ZONES_DIR "/lighting_control_state"
#define HEAD_PATH RGB_ZONES_DIR "/zone00"
#define LEFT_PATH RGB_ZONES_DIR "/zone01"
#define RIGHT_PATH RGB_ZONES_DIR "/zone02"

char* get_zone_path(Zone zone);
int parse_uint(const char* str, unsigned long* out);
// int get_str_between(char* str, size_t size, char start, char end, char out[], char* endp);

void check_platform_exists(void) {
	char* filepath = PLATFORM_DIR;
	DIR* fd = opendir(filepath);
	if (!fd) {
		fprintf(stderr, "Error: failed to open %s - %s\n", PLATFORM_DIR, strerror(errno));
		exit(1);
	}
	closedir(fd);
}

void check_rgb_zones_exists(void) {
	char* filepath = RGB_ZONES_DIR;
	DIR* fd = opendir(filepath);
	if (!fd) {
		fprintf(stderr, "Error: failed to open %s - %s\n", filepath, strerror(errno));
		exit(1);
	}
	closedir(fd);
}

char* get_zone_path(Zone zone) {
	char* zone_path = NULL;
	switch (zone) {
		case head:
			zone_path = HEAD_PATH;
			break;
		case left:
			zone_path = LEFT_PATH;
			break;
		case right:
			zone_path = RIGHT_PATH;
			break;
		default: // shouldn't happen
			zone_path = NULL;
			break;
	}
	if (zone_path == NULL) {
		fprintf(stderr, "zone_path is NULL! this shouldn't happen.");
		exit(1);
	}
	return zone_path;
}

void set_rgb_zone_color(RGBZoneColor color) {
	check_rgb_zones_exists();
	char* zone_path = get_zone_path(color.zone);
	// printf("zone_path: %s\n", zone_path);
	int fd = open(zone_path, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Error: failed to open %s - %s\n", zone_path, strerror(errno));
		exit(1);
	}
	// set_lighting_control_state(running); // i'm not really sure whether this changes anything, but it seems that Steam OS's original script for alienware_wmi control does this
	char writebuf[8];
	snprintf(writebuf, sizeof(writebuf), "%02x%02x%02x\n", color.red, color.green, color.blue);
	// printf("size: %lu -> %s", sizeof(writebuf), writebuf);
	write(fd, writebuf, sizeof(writebuf));
	close(fd);
}

int parse_uint(const char* str, unsigned long* out) {
	// printf("parsing text: %s\n", str);
	char* endp;
	errno = 0;
	unsigned long num = strtoul(str, &endp, 10);
	if (endp == str) {
		fprintf(stderr, "Error: no digits found - %s", strerror(errno));
		return -1;
	}
	if (errno == ERANGE) {
		fprintf(stderr, "Error: resulting value out of range - %s", strerror(errno));
		return -1;
	}
	if (*endp != '\0') {
		fprintf(stderr, "Error: trailing characters found at the end\n");
		// return -1;
	}
	*out = num;
	return 0;
}

int get_str_between(char* str, size_t size, char start, char end, char out[], char** endp) {
	// printf("get between '%c' and '%c'\n", start, end);
	void *colonchar = memchr(str, start, size);
	void *commachar = NULL;
	if (colonchar == NULL) {
		fprintf(stderr, "Error: failed to find '%c' character\n", start);
		return -1;
	}
	size_t len = (char*)colonchar - str;
	// printf("str len: %lu, size: %zu, str: %s", strlen(str), size, str);
	commachar = memchr(colonchar, end, size - len);
	if (commachar == NULL) {
		fprintf(stderr, "Error: failed to find '%c' character\n", end);
		return -1;
	}
	// printf("%c\n", *((char*)colonchar));
	// printf("%s\n", ((char*)commachar));
	size_t colorstrlen = (char*)commachar - 1 - (char*)colonchar;
	// printf(": - , -> %ld\n", colorstrlen);
	// it has to be <= 4 since colorbuf size is 5
	// it takes in cosiderarion a blank space plus a 3 digit number
	// max of ' 255' (8 bits)
	// in reality for inspiron 5675 it will be just ' 15' max
	if (colorstrlen > 4) {
		fprintf(stderr, "something went wrong while reading the color levels");
	}
	memcpy(out, (char*)colonchar+1, colorstrlen);
	out[colorstrlen] = '\0';
	// printf("commachar: %s", (char*)commachar);
	*endp = (char*)commachar;
	// printf("str value: %s\n", out);
	return 0;
}

RGBZoneColor get_rgb_zone_color(Zone zone) {
	check_rgb_zones_exists();
	char* zone_path = get_zone_path(zone);
	int fd = open(zone_path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error: failed to open %s - %s\n", zone_path, strerror(errno));
		exit(1);
	}
	char buf[30];
	ssize_t bytesread = read(fd, buf, sizeof(buf) - 1);
	if (bytesread < 0) {
		fprintf(stderr, "Error: failed to read %s - %s", zone_path, strerror(errno));
		exit(1);
	}
	buf[bytesread] = '\0';
	// printf("%s", buf);
	RGBZoneColor zonecolor;
	zonecolor.zone = zone;
	char colorbuf[5];
	char* endp = NULL;
	int status = get_str_between(buf, bytesread, ':', ',', colorbuf, &endp);
	if (status < 0) {
		fprintf(stderr, "error get_str_between - status < 0\n");
		exit(1);
	}
	if (endp == NULL) {
		fprintf(stderr, "error get_str_between - endp == NULL\n");
		exit(1);
	}
	unsigned long value = -1;
	status = parse_uint(colorbuf, &value);
	if (status < 0 || value == -1) {
		fprintf(stderr, "error get_str_between\n");
		exit(1);
	}
	// printf("red str value: %s\n", colorbuf);
	// printf("red int value: %lu\n", value);
	zonecolor.red = value;
	char* greenchar = endp;
	size_t len = endp - buf;
	endp = NULL;
	status = get_str_between(greenchar, bytesread - len, ':', ',', colorbuf, &endp);
	if (status < 0 || endp == NULL) {
		exit(1);
	}
	value = -1;
	status = parse_uint(colorbuf, &value);
	if (status < 0 || value == -1) {
		exit(1);
	}
	// printf("green str value: %s\n", colorbuf);
	// printf("green int value: %lu\n", value);
	zonecolor.green = value;
	char* bluechar = endp;
	len = endp - buf;
	endp = NULL;
	status = get_str_between(bluechar, 1 + bytesread - len, ':', '\n', colorbuf, &endp);
	if (status < 0 || endp == NULL) {
		exit(1);
	}
	value = -1;
	status = parse_uint(colorbuf, &value);
	if (status < 0 || value == -1) {
		exit(1);
	}
	// printf("blue str value: %s\n", colorbuf);
	// printf("blue int value: %lu\n", value);
	zonecolor.blue = value;
	return zonecolor;
}

void set_lighting_control_state(LightingControlState state) {
	char* statebuf = NULL;
	switch (state) {
		case booting:
			statebuf = "booting\n";
			break;
		case running:
			statebuf = "running\n";
			break;
		case suspend:
			statebuf = "suspend\n";
			break;
		default: // shouldn't happen
			statebuf = NULL;
			break;
	}
	if (statebuf == NULL) {
		fprintf(stderr, "statebuf is NULL! this shouldn't happen.");
		exit(1);
	}
	char* lighting_control_state_path = LIGHTING_CONTROL_STATE_PATH;
	// printf("lighting_control_state_path: %s\n", lighting_control_state_path);
	int fd = open(lighting_control_state_path, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Error: failed to open %s - %s\n", lighting_control_state_path, strerror(errno));
		exit(1);
	}
	// printf("size: %lu -> %s", strlen(statebuf), statebuf);
	write(fd, statebuf, strlen(statebuf)+1); // i'm not sure if i really need to write the '\0' character
	close(fd);
}
