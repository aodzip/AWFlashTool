#pragma once
#include <lvgl/lvgl.h>
#include <vendor/libtmt/tmt.h>

struct terminalemulator_t;

typedef void (*terminalemulator_cb_t)(struct terminalemulator_t *instance, bool fc);

typedef struct terminalemulator_t
{
    struct
    {
        char exe[256];
        terminalemulator_cb_t cb;
        lv_style_t style_canvas_text;
    } config;
    int pid_child;
    int pty_master;
    TMT *libtmt;
    lv_obj_t *cont_main;
    lv_color_t *canvas_buffer;
    lv_obj_t *canvas_terminal;
    lv_coord_t x_terminal, y_terminal;
    lv_task_t *task_refresh;
} terminalemulator_t;

void TerminalEmulator_open(lv_obj_t *parent, terminalemulator_t *config);
void TerminalEmulator_close(terminalemulator_t *config);
