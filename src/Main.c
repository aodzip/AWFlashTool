#include <helper/SystemHelper.h>
#include <helper/LittlevGLHelper.h>
#include <UI/UI.h>

int main()
{
    SystemHelper_backlight_set(8);
    LittlevGL_init();
    UI_create();
    LittlevGL_loop();
}
