#pragma once

#include "ezmidi/ezmidi.h"

#include <queue>
#include <mutex>
#include <cstdint>

namespace ezmidi {

/**
Processes and filters raw MIDI messages.
Relevant messages are queued as Ezmidi_Event event objects.
This class is designed to be thread safe.
*/
class MessageProcessor{
public:
    void processMidiMessage(const uint8_t* data);
	int getNextEvent(Ezmidi_Event& event);
    
private:
    void processNoteEvent(const uint8_t* data);
	
private:
	std::queue<Ezmidi_Event> events_;
	std::mutex mutex_;
};
	
}
