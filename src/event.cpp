#include "event.h"
#include "midi.h"

namespace Ezmidi {
    
void EventQueue::processMessage(const uint8_t* data)
{
    int midiStatus = data[0];
        
    if (Midi::shouldFilterEvent(midiStatus)) {
        return;
    }
    else if (Midi::isNoteEvent(midiStatus)) {            
        processNoteEvent(data);
    }
}
	
void EventQueue::processNoteEvent(const uint8_t* data)
{
	int status = data[0];

	Ezmidi_Event event;
	event.note_event.type = Ezmidi_EventType::EZMIDI_EVENT_NOTE;
	event.note_event.note = data[1];
	event.note_event.velocity = data[2];
	
	// Note: note on with a velocity of 0 means that the note was released!
	if (status >= Midi::Status::NoteOnChannel1 && status <= Midi::Status::NoteOnChannel16) {
		event.note_event.detail = event.note_event.velocity > 0 ? EZMIDI_NOTEEVENT_ON : EZMIDI_NOTEEVENT_OFF;
	}
	else{
		event.note_event.detail = EZMIDI_NOTEEVENT_OFF;
	}
	
	event_queue.push(event);
}

int EventQueue::pumpEvents(Ezmidi_Event& event)
{	
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
