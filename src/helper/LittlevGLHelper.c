#include "LittlevGLHelper.h"
#include "LittlevGLHelper.private.h"
#include <Config.h>
#include <unistd.h>
#include <pthread.h>

#ifdef PC_DEBUG
#include <SDL2/SDL.h>
#include <lv_drivers/display/monitor.h>
#include <lv_drivers/indev/mouse.h>
#else
#include <lv_drivers/display/fbdev.h>
#include <lv_drivers/indev/evdev.h>
#endif

#define DISP_BUF_SIZE 15 * 1024

lv_font_t lv_default_ttf_font;

static pthread_t thread_tick;

#ifdef PC_DEBUG
static int tick_thread(void *data);
#else
static void *tick_thread(void *data);
#endif

void LittlevGL_init()
{
    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    lv_default_ttf_font = lv_font_roboto_16;
    lv_init();

#ifdef PC_DEBUG
    monitor_init();

    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = monitor_flush;
    lv_disp_drv_register(&disp_drv);
#else
    fbdev_init();

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.rotated = 1;
    lv_disp_drv_register(&disp_drv);

    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);
#endif

#ifdef PC_DEBUG
    mouse_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv);

    LV_IMG_DECLARE(mouse_cursor_icon);
    lv_obj_t *cursor_obj = lv_img_create(lv_disp_get_scr_act(NULL), NULL);
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(mouse_indev, cursor_obj);
#else
    evdev_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = evdev_read;
    lv_indev_drv_register(&indev_drv);
#endif

#ifdef PC_DEBUG
    SDL_CreateThread(tick_thread, "tick", NULL);
#else
    pthread_create(&thread_tick, NULL, tick_thread, NULL);
#endif
}

#ifdef PC_DEBUG
static int tick_thread(void *data)
#else
static void *tick_thread(void *data)
#endif
{
    (void)data;
    while (1)
    {
        lv_tick_inc(1);
#ifdef PC_DEBUG
        SDL_Delay(1);
#else
        usleep(1000);
#endif
    }
#ifdef PC_DEBUG
    return 0;
#else
    usleep(1000);
#endif
}

void LittlevGL_loop()
{
    while (1)
    {
        lv_task_handler();
        usleep(5000);
    }
}
