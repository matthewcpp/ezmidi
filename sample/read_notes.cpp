#include "ezmidi.h"

#include <iostream>
#include <thread>
#include <chrono>

void log_message(const char* message, void* user_data);

int main (int argc, char** argv)
{    
	Ezmidi_Config config;
	ezmidi_config_init(&config);
	config.log_func = log_message;

	ezmidi* ezmidi = ezmidi_create(&config);

	int source_count = ezmidi_get_source_count(ezmidi);
    
	if (source_count == 0) {
		std::cout << "No MIDI sources available" << std::endl;
		return 1;
	}

	std::cout << "MIDI Sources available: " << source_count << std::endl;

	for (int i = 0; i < source_count; i++) {
		std::cout << ezmidi_get_source_name(ezmidi, i) << std::endl;
	}

	ezmidi_connect_source(ezmidi, 0);

	std::cout << "Monitoring midi events.  Press Enter to stop." << std::endl;
	bool monitor_midi_events = true;

	std::thread t([&monitor_midi_events]() {
		std::cin.get();
		monitor_midi_events = false;
	});

	while (monitor_midi_events) {
		Ezmidi_Event event;

		while (ezmidi_pump_events(ezmidi, &event)) {
			if (event.type == EZMIDI_NOTE) {
				std::cout << (event.note_event.detail == EZMIDI_NOTEEVENT_ON ? "Note On: " : "Note Off: ") << event.note_event.note << " velocity: " << event.note_event.velocity << std::endl;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	if (t.joinable()) t.join();

	std::cout << "Shutting down ezmidi" << std::endl;
	ezmidi_destroy(ezmidi);

	std::cin.get();

	return 0;
}

void log_message(const char* message, void* user_data)
{
	std::cout << "ezmidi log message: " << message << std::endl;
}
