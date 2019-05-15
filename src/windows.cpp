#include "ezmidi.h"
#include "event.h"
#include "midi.h"

#include "windows.h"

#include <string>
#include <mutex>

struct EzmidiWindows
{
	HMIDIIN input_handle = NULL;
	Ezmidi::EventQueue event_queue;
	std::string return_data;
	std::mutex mutex;
	Ezmidi_Config config;
};

static void CALLBACK windows_midi_in_proc(HMIDIIN input_handle, UINT message_type, DWORD_PTR instance_data, DWORD_PTR message, DWORD timestamp)
{
	auto midi_lib = reinterpret_cast<EzmidiWindows*>(instance_data);
	const char* packet = reinterpret_cast<const char*>(&message);
	
	if (message_type != MIM_DATA) return;

	std::lock_guard<std::mutex> lock(midi_lib->mutex);
	midi_lib->event_queue.processMessage(reinterpret_cast<const unsigned char *>(&message));
}

void close_existing_connection(EzmidiWindows* midi_lib)
{
	if (midi_lib->input_handle == NULL) return;

	midiInReset(midi_lib->input_handle);
	midiInStop(midi_lib->input_handle);
	midiInClose(midi_lib->input_handle);

	midi_lib->input_handle = NULL;
}

Ezmidi_Context* ezmidi_create(Ezmidi_Config* config)
{
	auto midi_lib = new EzmidiWindows{};

	if (config)
		midi_lib->config = *config;
	else
		ezmidi_config_init(&midi_lib->config);

	return reinterpret_cast<Ezmidi_Context*>(midi_lib);
}

void ezmidi_destroy(Ezmidi_Context* context)
{
	auto midi_lib = reinterpret_cast<EzmidiWindows*>(context);
	{
		std::lock_guard<std::mutex> lock(midi_lib->mutex);
		close_existing_connection(midi_lib);
	}
	
	delete midi_lib;
}

int ezmidi_get_source_count(Ezmidi_Context* /* context */)
{
	return static_cast<int>(midiInGetNumDevs());
}

const char* ezmidi_get_source_name(Ezmidi_Context* context, int source_index)
{
	int num_devices = static_cast<int>(midiInGetNumDevs());

	if (source_index < 0 || source_index >= num_devices) {
		return nullptr;
	}

	auto midi_lib = reinterpret_cast<EzmidiWindows*>(context);

	MIDIINCAPS source_info;
	midiInGetDevCaps(source_index, &source_info, sizeof(MIDIINCAPS));
	std::string name = source_info.szPname;
	midi_lib->return_data = name;

	return midi_lib->return_data.c_str();
}

void ezmidi_connect_source(Ezmidi_Context* context, int source)
{
	auto midi_lib = reinterpret_cast<EzmidiWindows*>(context);

	std::lock_guard<std::mutex> lock(midi_lib->mutex);

	int source_count = static_cast<int>(midiInGetNumDevs());
	if (source < 0 || source >= source_count) {
		return;
	}

	close_existing_connection(midi_lib);

	if (midi_lib->config.log_func) {
		std::string error_message = "opening connection to source: " + std::to_string(source);
		midi_lib->config.log_func(error_message.c_str(), midi_lib->config.user_data);
	}

	MMRESULT result = midiInOpen(&midi_lib->input_handle, source, reinterpret_cast<DWORD_PTR>(&windows_midi_in_proc),reinterpret_cast<DWORD_PTR>(midi_lib), CALLBACK_FUNCTION);

	if (result == MMSYSERR_NOERROR) {
		midiInStart(midi_lib->input_handle);
	}
	else {
		if (midi_lib->config.log_func) {
			std::string error_message = "Windows MM error connecting to source: " + std::to_string(result);
			midi_lib->config.log_func(error_message.c_str(), midi_lib->config.user_data);
		}

		midiInClose(midi_lib->input_handle);
	}
}

int ezmidi_pump_events(Ezmidi_Context* context, Ezmidi_Event* event)
{
	auto midi_lib = reinterpret_cast<EzmidiWindows*>(context);

	std::lock_guard<std::mutex> lock(midi_lib->mutex);

	if (event)
		return midi_lib->event_queue.pumpEvents(*event);
	else
		return 0;
}