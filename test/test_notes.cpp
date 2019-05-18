#include "gtest/gtest.h"
#include "event.h"
#include "ezmidi.h"

#include <array>

using MidiNodeMessage = std::array<uint8_t, 3>;

TEST(Notes, NoteDown) {
	MidiNodeMessage message = { 128, 60, 50 }; //middle c note on

	Ezmidi::EventQueue event_queue;
	event_queue.processMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(event_queue.pumpEvents(event), 0);
}