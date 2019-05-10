#include "ezmidi.h"
#include "event.h"
#include "midi.h"

#include "windows.h"

#include <any>

struct Ezmidi_Windows
{
	HMIDIIN input_handle;
	Ezmidi::EventQueue event_queue;
	std::any return_data;
	Ezmidi_Config config;
};

#define LOW_BYTE 0x000000FF

static void CALLBACK windows_midi_in_proc(HMIDIIN input_handle, UINT message_type, DWORD_PTR instance_data, DWORD_PTR message, DWORD timestamp)
{
	auto midi_lib = reinterpret_cast<Ezmidi_Windows*>(instance_data);
	const char* packet = reinterpret_cast<const char*>(&message);
	
	if (message_type != MIM_DATA) return;

	int status = static_cast<int>(message & LOW_BYTE);
	if (Midi::shouldFilterEvent(status)) return;

	if (Midi::isNoteEvent(status)) {
		int key = static_cast<int>((message >> 8) & LOW_BYTE);
		int velocity = static_cast<int>((message >> 16) & LOW_BYTE);

		midi_lib->event_queue.pushNote(status, key, velocity);
	}
}

ezmidi* ezmidi_create(Ezmidi_Config* config)
{
	auto midi_lib = new Ezmidi_Windows{};

	if (config)
		midi_lib->config = *config;
	else
		ezmidi_config_init(&midi_lib->config);

	return reinterpret_cast<ezmidi*>(midi_lib);
}

void ezmidi_destroy(ezmidi* context)
{
	auto midi_lib = reinterpret_cast<Ezmidi_Windows*>(context);

	if (midi_lib->input_handle) {
		midiInClose(midi_lib->input_handle);
	}

	delete midi_lib;
}

int ezmidi_get_source_count(ezmidi* context)
{
	return static_cast<int>(midiInGetNumDevs());
}

const char* ezmidi_get_source_name(ezmidi* context, int source_index)
{
	int num_devices = static_cast<int>(midiInGetNumDevs());

	if (source_index < 0 || source_index >= num_devices) {
		return nullptr;
	}

	auto midi_lib = reinterpret_cast<Ezmidi_Windows*>(context);

	MIDIINCAPS source_info;
	midiInGetDevCaps(source_index, &source_info, sizeof(MIDIINCAPS));
	std::string name = source_info.szPname;
	midi_lib->return_data = name;

	return std::any_cast<std::string&>(midi_lib->return_data).c_str();
}

void ezmidi_connect_source(ezmidi* context, int source)
{
	auto midi_lib = reinterpret_cast<Ezmidi_Windows*>(context);

	int source_count = static_cast<int>(midiInGetNumDevs());
	if (source < 0 || source >= source_count) {
		return;
	}

	MMRESULT result = midiInOpen(&midi_lib->input_handle, source, reinterpret_cast<DWORD_PTR>(&windows_midi_in_proc),reinterpret_cast<DWORD_PTR>(midi_lib), CALLBACK_FUNCTION);

	if (result == MMSYSERR_NOERROR) {
		midiInStart(midi_lib->input_handle);
	}
	else {
		midiInClose(midi_lib->input_handle);
	}
}

int ezmidi_pump_events(ezmidi* context, Ezmidi_Event* event)
{
	auto midi_lib = reinterpret_cast<Ezmidi_Windows*>(context);

	if (event)
		return midi_lib->event_queue.pumpEvents(*event);
	else
		return 0;
}