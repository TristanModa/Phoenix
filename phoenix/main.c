#include <vulfEngine.h>

#include "constants.h"
#include "phoenix.h"

void setAppProperties();

int main(void) {
    // Create the application
    Application_create(
        "Phoenix", "com.vulf.phoenix",
        (AppVersion){ 0, 0, 0 },
        Phoenix_init,
        Phoenix_destroy,
        Phoenix_update,
        Phoenix_tick,
        Phoenix_render);

    // Set the properties of all application systems
    setAppProperties();

    // Run the application
    Application_run();
}

void setAppProperties() {
    // Set logger properties
    const LoggerProperties loggerProperties = {
        .logFilePath = LOG_FILE_PATH
    };
    Logger_setProperties(&loggerProperties);

    // Set time properties
    const TimeProperties timeProperties = {
        .targetTicksPerSecond = TARGET_TICKS_PER_SECOND,
        .maxTickProcessingTimePerFrame = MAX_TICK_PROCESSING_TIME_PER_FRAME
    };
    Time_setProperties(&timeProperties);

    // Set input properties
    const InputProperties inputProperties = {
        .buttonCount = BUTTON_COUNT,
        .defaultBindings = DEFAULT_BINDINGS
    };
    Input_setProperties(&inputProperties);
}