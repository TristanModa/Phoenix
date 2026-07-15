#include <vulfEngine.h>
#include "phoenix.h"

int main(void) {
    // Create and run the application
    Application_create("Phoenix", (AppVersion){ 0, 0, 0 },
        Phoenix_init,
        Phoenix_destroy,
        Phoenix_update,
        Phoenix_tick,
        Phoenix_render);
    Application_run();
}