#include "ezmidi/ezmidi.h"
#include "message_processor.h"
#include "midi.h"

#include "gtest/gtest.h"

#include <array>

using MidiNoteMessage = std::array<uint8_t, 3>;

TEST(Queue, GetNextEmpty)
{
	ezmidi::MessageProcessor message_processor;

	Ezmidi_Event event;
	ASSERT_EQ(message_processor.getNextEvent(event), 0);
}

TEST(Queue, GetNext)
{
	MidiNoteMessage message = { Midi::Status::NoteOnChannel1, 60, 11 };

	ezmidi::MessageProcessor message_processor;
	message_processor.processMidiMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(message_processor.getNextEvent(event), 0);
	ASSERT_EQ(message_processor.getNextEvent(event), 0);
}

TEST(Queue, FIFO)
{
	ezmidi::MessageProcessor message_processor;

	MidiNoteMessage message1 = { Midi::Status::NoteOnChannel1, 60, 11 };
	MidiNoteMessage message2 = { Midi::Status::NoteOnChannel1, 61, 11 };

	message_processor.processMidiMessage(message1.data());
	message_processor.processMidiMessage(message2.data());

	Ezmidi_Event event;
	ASSERT_NE(message_processor.getNextEvent(event), 0);
	ASSERT_EQ(event.note_event.note, message1[1]);

	ASSERT_NE(message_processor.getNextEvent(event), 0);
	ASSERT_EQ(event.note_event.note, message2[1]);
}