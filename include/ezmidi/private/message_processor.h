#pragma once

#include "ezmidi/ezmidi.h"

#include <queue>
#include <mutex>
#include <cstdint>

namespace ezmidi {

/**
This class processes and filters midi events.
Relevant messages are queued as Ezmidi_Event event objects.
*/
class MessageProcessor{
public:
    void processMidiMessage(const uint8_t* data);
	int getNextEvent(Ezmidi_Event& event);
    
private:
    void processNoteEvent(const uint8_t* data);
	
private:
	std::queue<Ezmidi_Event> events_;
};
	
}
