#include "UI.h"
#include <stdio.h>
#include <stdbool.h>
#include <Config.h>

#include "Lib/TerminalEmulator.h"

static lv_obj_t *win_root;
static lv_obj_t *win_main;

static lv_obj_t *label_info;
static lv_obj_t *btn_fdisk;
static lv_obj_t *btn_uboot;
static lv_obj_t *btn_bootfs;
static lv_obj_t *btn_rootfs;

static lv_task_t *task_countdown_auto;
static void task_auto_handle(lv_task_t *task);
static int auto_countdown = 5;
static int enable_auto = false;
static int step_auto = 0;

static void win_main_callback(lv_obj_t *obj, lv_event_t event);
static void btn_fdisk_callback(lv_obj_t *obj, lv_event_t event);
static void btn_uboot_callback(lv_obj_t *obj, lv_event_t event);
static void btn_bootfs_callback(lv_obj_t *obj, lv_event_t event);
static void btn_rootfs_callback(lv_obj_t *obj, lv_event_t event);

static terminalemulator_t TE;
static void te_finished_callback(terminalemulator_t *instance, bool fc);

void UI_create()
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);
    lv_disp_load_scr(lv_obj_create(NULL, NULL));

    win_root = lv_cont_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(win_root, hres, vres);

    lv_theme_t *th = lv_theme_material_init(0, NULL);
    lv_theme_set_current(th);

    win_main = lv_cont_create(win_root, NULL);
    lv_obj_set_size(win_main, hres, vres);
    lv_obj_align(win_main, win_root, LV_ALIGN_CENTER, 0, 0);

    lv_cont_set_layout(win_main, LV_LAYOUT_PRETTY);
    lv_obj_set_event_cb(win_main, win_main_callback);

    label_info = lv_label_create(win_main, NULL);
    btn_fdisk = lv_btn_create(win_main, NULL);
    btn_uboot = lv_btn_create(win_main, NULL);
    btn_bootfs = lv_btn_create(win_main, NULL);
    btn_rootfs = lv_btn_create(win_main, NULL);

    lv_obj_set_parent_event(label_info, true);
    lv_obj_set_parent_event(btn_fdisk, true);
    lv_obj_set_parent_event(btn_uboot, true);
    lv_obj_set_parent_event(btn_bootfs, true);
    lv_obj_set_parent_event(btn_rootfs, true);

    lv_obj_set_width(label_info, lv_obj_get_width(win_main));
    lv_obj_set_width(btn_fdisk, lv_obj_get_width(win_main));
    lv_obj_set_width(btn_uboot, lv_obj_get_width(win_main));
    lv_obj_set_width(btn_bootfs, lv_obj_get_width(win_main));
    lv_obj_set_width(btn_rootfs, lv_obj_get_width(win_main));

    lv_obj_set_event_cb(btn_fdisk, btn_fdisk_callback);
    lv_obj_set_event_cb(btn_uboot, btn_uboot_callback);
    lv_obj_set_event_cb(btn_bootfs, btn_bootfs_callback);
    lv_obj_set_event_cb(btn_rootfs, btn_rootfs_callback);

    lv_label_set_text(label_info, "Waiting...");

    lv_obj_t *label_tmp;
    label_tmp = lv_label_create(btn_fdisk, NULL);
    lv_label_set_text(label_tmp, "Create MMC Partition");
    label_tmp = lv_label_create(btn_uboot, NULL);
    lv_label_set_text(label_tmp, "Flash U-Boot");
    label_tmp = lv_label_create(btn_bootfs, NULL);
    lv_label_set_text(label_tmp, "Update bootfs");
    label_tmp = lv_label_create(btn_rootfs, NULL);
    lv_label_set_text(label_tmp, "Update rootfs");

    task_countdown_auto = lv_task_create(task_auto_handle, 1000, LV_TASK_PRIO_MID, NULL);

    lv_style_copy(&TE.config.style_canvas_text, &lv_style_plain);
    TE.config.style_canvas_text.body.grad_color = LV_COLOR_BLACK;
    TE.config.style_canvas_text.body.main_color = LV_COLOR_BLACK;
    TE.config.style_canvas_text.text.font = &lv_font_roboto_12;
    TE.config.style_canvas_text.text.color = LV_COLOR_WHITE;
    TE.config.cb = te_finished_callback;
}

static void task_auto_handle(lv_task_t *task)
{
    if (auto_countdown--)
    {
        char buffer[128] = {0};
        snprintf(buffer, sizeof(buffer) - 1, "%d seconds to auto flash...", auto_countdown);
        lv_label_set_text(label_info, buffer);
        printf("%s\n", buffer);
    }
    else
    {
        lv_label_set_text(label_info, "Starting auto flash...");
        printf("%s\n", "Starting auto flash...");
        lv_task_del(task_countdown_auto);
        enable_auto = true;
        te_finished_callback(&TE, false);
    }
}

static void te_finished_callback(terminalemulator_t *instance, bool fc)
{
    TerminalEmulator_close(instance);
    if (enable_auto)
    {
        switch (step_auto)
        {
        case 0:
            btn_fdisk_callback(btn_fdisk, LV_EVENT_CLICKED);
            break;
        case 1:
            btn_uboot_callback(btn_uboot, LV_EVENT_CLICKED);
            break;
        case 2:
            btn_bootfs_callback(btn_bootfs, LV_EVENT_CLICKED);
            break;
        case 3:
            btn_rootfs_callback(btn_rootfs, LV_EVENT_CLICKED);
            break;
        default:
            enable_auto = false;
            lv_label_set_text(label_info, "Done");
        }
    }
}

static void win_main_callback(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        lv_label_set_text(label_info, "Manual Mode");
        if (task_countdown_auto)
        {
            lv_task_del(task_countdown_auto);
            task_countdown_auto = NULL;
        }
    }
}

static void btn_fdisk_callback(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (enable_auto)
        {
            step_auto++;
        }
        strncpy(TE.config.exe, SCRIPT_DIR "fdisk.sh", sizeof(TE.config.exe) - 1);
        TerminalEmulator_open(win_root, &TE);
    }
}

static void btn_uboot_callback(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (enable_auto)
        {
            step_auto++;
        }
        strncpy(TE.config.exe, SCRIPT_DIR "uboot.sh", sizeof(TE.config.exe) - 1);
        TerminalEmulator_open(win_root, &TE);
    }
}

static void btn_bootfs_callback(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (enable_auto)
        {
            step_auto++;
        }
        strncpy(TE.config.exe, SCRIPT_DIR "bootfs.sh", sizeof(TE.config.exe) - 1);
        TerminalEmulator_open(win_root, &TE);
    }
}

static void btn_rootfs_callback(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (enable_auto)
        {
            step_auto++;
        }
        strncpy(TE.config.exe, SCRIPT_DIR "rootfs.sh", sizeof(TE.config.exe) - 1);
        TerminalEmulator_open(win_root, &TE);
    }
}
