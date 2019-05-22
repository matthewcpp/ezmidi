#pragma once

#include <ezmidi/ezmidi.h>

#ifdef __cplusplus
extern "C" {
#endif

void sleep_for(int milliseconds);
char* event_to_string(Ezmidi_Event* event);

#ifdef __cplusplus
}
#endif