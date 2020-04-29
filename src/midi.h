#ifndef EZMIDI_MIDI_H
#define EZMIDI_MIDI_H

namespace Midi {

// reference: https://www.midi.org/specifications-old/item/table-2-expanded-messages-list-status-bytes
enum Status {
	NoteOffChannel1 = 128,
	NoteOffChannel16 = 143,
	NoteOnChannel1 = 144,
	NoteOnChannel16 = 159,
	TimingClock = 248,
	ActiveSensing = 254
};

inline bool shouldFilterEvent(int status) {
	return status == Status::ActiveSensing || status == Status::TimingClock;
}
	
inline bool isNoteEvent(int status) {
	return status >= Status::NoteOffChannel1 && status <= Status::NoteOnChannel16;
}

}

#endif
