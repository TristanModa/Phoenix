#ifndef ENGINE_DEBUG_DEBUGPANEL_H
#define ENGINE_DEBUG_DEBUGPANEL_H

#include <stddef.h>

constexpr size_t WINDOW_TITLE_SIZE = 64;
constexpr float WINDOW_TOGGLE_ANIM_DURATION = 0.25f;

void DebugPanel_init(const char* appName, const char* appVersion);

void DebugPanel_update();

#endif //ENGINE_DEBUG_DEBUGPANEL_H
