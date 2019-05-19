#include "ezmidi/ezmidi.h"
#include "ezmidi/private/event.h"
#include "ezmidi/private/midi.h"

#include "gtest/gtest.h"

#include <array>

using MidiNoteMessage = std::array<uint8_t, 3>;

TEST(Queue, PumpEmpty)
{
	Ezmidi::EventQueue event_queue;

	Ezmidi_Event event;
	ASSERT_EQ(event_queue.pumpEvents(event), 0);
}

TEST(Queue, Pump)
{
	MidiNoteMessage message = { Midi::Status::NoteOnChannel1, 60, 11 };

	Ezmidi::EventQueue event_queue;
	event_queue.processMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(event_queue.pumpEvents(event), 0);
	ASSERT_EQ(event_queue.pumpEvents(event), 0);
}

TEST(Queue, FIFO)
{
	Ezmidi::EventQueue event_queue;

	MidiNoteMessage message1 = { Midi::Status::NoteOnChannel1, 60, 11 };
	MidiNoteMessage message2 = { Midi::Status::NoteOnChannel1, 61, 11 };

	event_queue.processMessage(message1.data());
	event_queue.processMessage(message2.data());

	Ezmidi_Event event;
	ASSERT_NE(event_queue.pumpEvents(event), 0);
	ASSERT_EQ(event.note_event.note, message1[1]);

	ASSERT_NE(event_queue.pumpEvents(event), 0);
	ASSERT_EQ(event.note_event.note, message2[1]);
}