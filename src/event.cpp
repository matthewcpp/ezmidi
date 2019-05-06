#include "event.h"
#include "midi.h"

namespace Ezmidi {
	
void EventQueue::pushNote(int status, int key, int velocity)
{
	Ezmidi_Event event;
	event.note_event.type = Ezmidi_EventType::EZMIDI_NOTE;
	event.note_event.velocity = velocity;
	event.note_event.note = key;
	
	// Note: note on with a velocity of 0 means that the note was released!
	if (status >= Midi::Status::NoteOnChannel1 && status <= Midi::Status::NoteOnChannel16) {
		event.note_event.detail = velocity > 0 ? EZMIDI_NOTEEVENT_ON : EZMIDI_NOTEEVENT_OFF;
	}
	else{
		event.note_event.detail = EZMIDI_NOTEEVENT_OFF;
	}
	
	std::lock_guard<std::mutex> lock(event_mutex);
	event_queue.push(event);
}

int EventQueue::pumpEvents(Ezmidi_Event& event)
{
	std::lock_guard<std::mutex> lock(event_mutex);
	
	if (!event_queue.empty()) {
		event = event_queue.front();
		event_queue.pop();
		return 1;
	}
	else {
		return 0;
	}
}
	
}
