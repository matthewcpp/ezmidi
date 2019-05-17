#include "ezmidi.h"

#include "event.h"
#include "midi.h"

#include <alsa/asoundlib.h>

#include <mutex>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>
#include <cstdint>

struct EzmidiAlsa
{
    snd_rawmidi_t* midi_input = nullptr;
    Ezmidi_Config config;
    Ezmidi::EventQueue event_queue;
    std::mutex mutex;
    std::thread input_thread;
    std::atomic_bool process_input;
    std::string return_data;
};

using SourceDeviceVector = std::vector<std::pair<std::string, std::string>>;
SourceDeviceVector get_source_device_info();


Ezmidi_Context* ezmidi_create(Ezmidi_Config* config)
{
    auto ezmidi_alsa = new EzmidiAlsa{};
    ezmidi_alsa->process_input = false;

    if (config) {
        ezmidi_alsa->config = *config;
    }
    else {
        ezmidi_config_init(&ezmidi_alsa->config);
    }

    return reinterpret_cast<Ezmidi_Context*>(ezmidi_alsa);
}

Ezmidi_Error close_active_connection(EzmidiAlsa* ezmidi_alsa)
{
	if (ezmidi_alsa->midi_input) {
		ezmidi_alsa->process_input = false;

		if (ezmidi_alsa->input_thread.joinable()) {
			ezmidi_alsa->input_thread.join();
		}

		snd_rawmidi_close(ezmidi_alsa->midi_input);
		ezmidi_alsa->midi_input = nullptr;

		return EZMIDI_ERROR_NONE;
	}

	return EZMIDI_ERROR_NO_SOURCE_CONNECTED;
}

void ezmidi_destroy(Ezmidi_Context* context)
{
    auto ezmidi_alsa = reinterpret_cast<EzmidiAlsa*>(context);

	close_active_connection(ezmidi_alsa);

    delete ezmidi_alsa;
}

int ezmidi_get_source_count(Ezmidi_Context* /* context */)
{
    SourceDeviceVector devices = get_source_device_info();

    return static_cast<int>(devices.size());
}

const char* ezmidi_get_source_name(Ezmidi_Context* context, int source_index)
{
    auto ezmidi_alsa = reinterpret_cast<EzmidiAlsa*>(context);

    if (source_index >= 0) {
        SourceDeviceVector devices = get_source_device_info();

        if (source_index < devices.size()) {
            ezmidi_alsa->return_data = devices[source_index].second;

            return ezmidi_alsa->return_data.c_str();
        }
    }

    return nullptr;
}

void alsa_input_thread(EzmidiAlsa *ezmidi_alsa)
{
    ezmidi_alsa->process_input = true;

    uint8_t buffer[256] = {0};

    while (ezmidi_alsa->process_input) {
        int status = 0;

        while (status != -EAGAIN) {
            status = snd_rawmidi_read(ezmidi_alsa->midi_input, buffer, 256);

            if (status > 0) {
				std::lock_guard<std::mutex> lock(ezmidi_alsa->mutex);
				ezmidi_alsa->event_queue.processMessage(buffer);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

Ezmidi_Error ezmidi_connect_source(Ezmidi_Context* context, int source)
{
    auto ezmidi_alsa = reinterpret_cast<EzmidiAlsa*>(context);

    int open_mode = SND_RAWMIDI_NONBLOCK;
    SourceDeviceVector devices = get_source_device_info();
	if (source < 0 || source >= devices.size()) {
		return EZMIDI_ERROR_INVALID_SOURCE;
	}
	
    int status = snd_rawmidi_open(&ezmidi_alsa->midi_input, NULL, devices[source].first.c_str(), open_mode);
	
	if (status < 0) {
		return EZMIDI_ERROR_CONNECTION_FAILED;
	}

    ezmidi_alsa->input_thread = std::thread(alsa_input_thread, ezmidi_alsa);
	
	return EZMIDI_ERROR_NONE;
}

Ezmidi_Error ezmidi_disconnect_source(Ezmidi_Context* context)
{
	auto ezmidi_alsa = reinterpret_cast<EzmidiAlsa*>(context);
	
	return close_active_connection(ezmidi_alsa);
}

int ezmidi_has_source_connected(Ezmidi_Context* context)
{
	auto ezmidi_alsa = reinterpret_cast<EzmidiAlsa*>(context);

	return ezmidi_alsa->midi_input != nullptr;
}

int ezmidi_pump_events(Ezmidi_Context* context, Ezmidi_Event* event)
{
    if (event) {
        auto ezmidi_alsa = reinterpret_cast<EzmidiAlsa*>(context);
        std::lock_guard<std::mutex> lock(ezmidi_alsa->mutex);

        return ezmidi_alsa->event_queue.pumpEvents(*event);
    }

    return 0;
}

SourceDeviceVector get_source_device_info()
{
    SourceDeviceVector input_device_info;

    int card = -1;
    snd_card_next(&card);

    while (card >= 0) {
        snd_ctl_t *ctl;
        int device = -1;

        char sound_card_handle[32];
        snprintf(sound_card_handle, 32, "hw:%d", card);
        snd_ctl_open(&ctl, sound_card_handle, 0);

        snd_ctl_rawmidi_next_device(ctl, &device);

        while (device >= 0) {
            snd_rawmidi_info_t *info;
            int input_subdevices = 0;
            const char *sub_name;

            snd_rawmidi_info_alloca(&info);
            snd_rawmidi_info_set_device(info, device);

            snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
            snd_ctl_rawmidi_info(ctl, info);
            input_subdevices = snd_rawmidi_info_get_subdevices_count(info);

            for (int subdevice = 0; subdevice < input_subdevices; subdevice++) {
                snd_rawmidi_info_set_subdevice(info, subdevice);

                if (snd_ctl_rawmidi_info(ctl, info) == 0) {
                    char subdevice_handle[32];
                    snprintf(subdevice_handle, 32, "hw:%d,%d,%d", card, device, subdevice);
                    sub_name = snd_rawmidi_info_get_subdevice_name(info);

                    input_device_info.emplace_back(std::make_pair(subdevice_handle, sub_name));
                }
            }

            snd_ctl_rawmidi_next_device(ctl, &device);
        }

        snd_ctl_close(ctl);
        snd_card_next(&card);
    }

    return input_device_info;
}
