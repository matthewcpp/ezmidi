#pragma once

#include "ezmidi.h"

#include <queue>
#include <mutex>

namespace Ezmidi {
	
class EventQueue{
public:
	void pushNote(int status, int key, int velocity);
	int pumpEvents(Ezmidi_Event& event);
	
private:
	std::queue<Ezmidi_Event> event_queue;
};
	
}
