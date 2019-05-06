#include "ezmidi.h"

#include <iostream>
#include <thread>
#include <chrono>

bool monitor_midi_events = true;
void midi_thread();

int main (int argc, char** argv)
{
	int source_count = -50;
	ezmidi_get_source_count(&source_count);
	
	if (source_count == 0){
		std::cout << "No MIDI sources available" << std::endl;
		return 0;
	}
	
	std::cout << "MIDI Sources available: " << source_count << std::endl;
	std::thread midi_monitoring_thread(midi_thread);
	
	std::cin.get();
	monitor_midi_events = false;
	
	midi_monitoring_thread.join();

	return 0;
}

void midi_thread()
{
	ezmidi* ezmidi = ezmidi_create();
	ezmidi_connect_source(ezmidi, 0);
	
	std::cout << "monitoring midi messages" << std::endl;
	
	while (monitor_midi_events) {
		Ezmidi_Event event;
		
		while (ezmidi_pump_events(ezmidi, &event)){
			if (event.type == EZMIDI_NOTE) {
				std::cout << (event.note_event.detail == EZMIDI_NOTEEVENT_ON ? "Note On: " : "Note Off: ") << event.note_event.note << " velocity: " << event.note_event.velocity << std::endl;
			}
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
