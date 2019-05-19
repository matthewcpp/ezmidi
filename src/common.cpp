#include "ezmidi/ezmidi.h"

void ezmidi_config_init(Ezmidi_Config* config)
{
    config->log_func = nullptr;
	config->user_data = nullptr;
}
