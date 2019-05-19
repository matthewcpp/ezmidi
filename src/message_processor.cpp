#include "ezmidi/private/message_processor.h"
#include "ezmidi/private/midi.h"

namespace ezmidi {
    
void MessageProcessor::processMidiMessage(const uint8_t* data)
{
    int midiStatus = data[0];
        
    if (Midi::shouldFilterEvent(midiStatus)) {
        return;
    }
    else if (Midi::isNoteEvent(midiStatus)) {            
        processNoteEvent(data);
    }
}
	
void MessageProcessor::processNoteEvent(const uint8_t* data)
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
	
	events_.push(event);
}

int MessageProcessor::getNextEvent(Ezmidi_Event& event)
{	
	if (events_.empty()) {
		return 0;
	}

	event = events_.front();
	events_.pop();
	return 1;
}
	
}
