#define _GNU_SOURCE
#include "TerminalEmulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pty.h>
#include <termios.h>
#include <signal.h>

void libtmt_callback(tmt_msg_t m, TMT *vt, const void *a, void *p);
static void task_stream_handle(lv_task_t *task);
static void close_proc(terminalemulator_t *instance, bool fc);

void TerminalEmulator_open(lv_obj_t *parent, terminalemulator_t *instance)
{
    instance->pid_child = forkpty(&instance->pty_master, NULL, NULL, NULL);
    if (instance->pid_child < 0)
    {
        return;
    }
    else if (instance->pid_child == 0)
    {
        execl("/bin/sh", "sh", "-c", instance->config.exe, (char *)0);
        exit(-127);
    }
    else
    {
        instance->cont_main = lv_cont_create(parent, NULL);
        lv_cont_set_fit2(instance->cont_main, LV_FIT_FLOOD, LV_FIT_FLOOD);

        instance->canvas_terminal = lv_canvas_create(instance->cont_main, NULL);

        lv_obj_set_size(instance->canvas_terminal, lv_obj_get_width(instance->cont_main), lv_obj_get_height(instance->cont_main));

        instance->canvas_buffer = calloc(sizeof(lv_color_t), LV_CANVAS_BUF_SIZE_TRUE_COLOR(lv_obj_get_width(instance->canvas_terminal), lv_obj_get_height(instance->canvas_terminal)));

        lv_canvas_set_buffer(instance->canvas_terminal, instance->canvas_buffer, lv_obj_get_width(instance->canvas_terminal), lv_obj_get_height(instance->canvas_terminal), LV_IMG_CF_TRUE_COLOR);

        lv_canvas_fill_bg(instance->canvas_terminal, LV_COLOR_BLACK);

        fcntl(instance->pty_master, F_SETFL, O_NONBLOCK);
        lv_coord_t font_h = lv_font_get_line_height(instance->config.style_canvas_text.text.font);
        uint16_t nline = lv_obj_get_height(instance->canvas_terminal) / font_h;
        uint16_t ncolumn = lv_obj_get_width(instance->canvas_terminal) / font_h * 2;
        instance->libtmt = tmt_open(nline, ncolumn, libtmt_callback, instance, NULL);

        instance->task_refresh = lv_task_create(task_stream_handle, 5, LV_TASK_PRIO_MID, instance);
    }
}

static void close_proc(terminalemulator_t *instance, bool fc)
{
    if (instance->pid_child)
    {
        kill(instance->pid_child, 9);
        instance->pid_child = 0;
        close(instance->pty_master);
        instance->pty_master = 0;
        tmt_close(instance->libtmt);
        instance->libtmt = NULL;
        lv_task_del(instance->task_refresh);
        instance->task_refresh = NULL;
        if (instance->config.cb)
        {
            instance->config.cb(instance, fc);
        }
    }
}

static void task_stream_handle(lv_task_t *task)
{
    terminalemulator_t *instance = task->user_data;
    char buf[128] = {0};
    ssize_t r = read(instance->pty_master, buf, sizeof(buf) - 1);
    if (r == -1 && errno == EAGAIN)
    {
        return;
    }
    else if (r > 0)
    {
        tmt_write(instance->libtmt, buf, 0);
        printf("%s", buf);
    }
    else
    {
        close_proc(instance, false);
    }
}

void libtmt_callback(tmt_msg_t m, TMT *vt, const void *a, void *p)
{
    terminalemulator_t *instance = p;
    lv_coord_t font_h = lv_font_get_line_height(instance->config.style_canvas_text.text.font);

    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);
    switch (m)
    {
    case TMT_MSG_BELL:
        printf("bing!\n");
        break;

    case TMT_MSG_UPDATE:
        for (size_t r = 0; r < s->nline; r++)
        {
            if (s->lines[r]->dirty)
            {
                instance->y_terminal = r * font_h;
                instance->x_terminal = 0;
                for (size_t c = 0; c < s->ncol; c++)
                {
                    char buf[2] = {0};
                    buf[0] = s->lines[r]->chars[c].c;
                    lv_canvas_draw_rect(instance->canvas_terminal, instance->x_terminal, instance->y_terminal, font_h, font_h, &instance->config.style_canvas_text);
                    lv_canvas_draw_text(instance->canvas_terminal, instance->x_terminal, instance->y_terminal, font_h, &instance->config.style_canvas_text, buf, LV_LABEL_ALIGN_LEFT);
                    instance->x_terminal += font_h / 2;
                }
            }
        }
        tmt_clean(vt);
        break;

    case TMT_MSG_ANSWER:
        printf("terminal answered %s\n", (const char *)a);
        break;

    case TMT_MSG_MOVED:
    {
        instance->x_terminal = c->c * font_h;
        instance->y_terminal = c->r * font_h;
    }
    break;
    }
}

void TerminalEmulator_close(terminalemulator_t *instance)
{
    close_proc(instance, true);
    if (instance->cont_main)
    {
        lv_obj_del(instance->cont_main);
    }
    if (instance->canvas_buffer)
    {
        free(instance->canvas_buffer);
    }
}
