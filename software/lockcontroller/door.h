#include "door-config.h"
#include "config.h"
#if DOOR_MODEL == DOOR_MODEL_1
#include "door-1.h"
#elif DOOR_MODEL == DOOR_MODEL_2
#include "door-2.h"
#endif
