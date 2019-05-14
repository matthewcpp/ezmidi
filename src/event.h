#pragma once

#include "ezmidi.h"

#include <queue>
#include <mutex>
#include <cstdint>

namespace Ezmidi {
	
class EventQueue{
public:
    void processMessage(const uint8_t* data);
	int pumpEvents(Ezmidi_Event& event);
    
private:
    void processNoteEvent(const uint8_t* data);
	
private:
	std::queue<Ezmidi_Event> event_queue;
};
	
}
