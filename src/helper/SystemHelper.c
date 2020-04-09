#include "SystemHelper.h"
#include <stdio.h>
#include <stdlib.h>

static int randInRange(int min, int max)
{
    double scale = 1.0 / RAND_MAX;
    double range = max - min + 1;
    return min + (int)(rand() * scale * range);
}

uint8_t SystemHelper_random_byte()
{
    return (uint8_t)randInRange(0, 255);
}

void SystemHelper_backlight_set(uint8_t level)
{
    level = level > 8 ? 8 : level;
    level = level < 1 ? 1 : level;
#ifndef PC_DEBUG
    char buf[2];
    snprintf(buf, 2, "%u", level);
    FILE *bk = fopen("/sys/class/backlight/backlight/brightness", "w");
    fwrite(buf, sizeof(buf), 1, bk);
    fclose(bk);
#endif
}

uint8_t SystemHelper_backlight_get()
{
#ifndef PC_DEBUG
    char buf[2];
    FILE *bk = fopen("/sys/class/backlight/backlight/brightness", "r");
    fread(buf, sizeof(buf), 1, bk);
    fclose(bk);
    return atoi(buf);
#endif
}
