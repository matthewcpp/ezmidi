#include "ezmidi/ezmidi.h"
#include "ezmidi/private/midi.h"
#include "ezmidi/private/message_processor.h"

#include "gtest/gtest.h"

#include <array>

using MidiNoteMessage = std::array<uint8_t, 3>;

TEST(Notes, NoteOn) 
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 50;
	MidiNoteMessage message = { Midi::Status::NoteOnChannel1, note, velocity };

	ezmidi::MessageProcessor message_processor;
	message_processor.processMidiMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(message_processor.getNextEvent(event), 0);

	ASSERT_EQ(event.type, EZMIDI_EVENT_NOTE);
	ASSERT_EQ(event.note_event.detail, EZMIDI_NOTEEVENT_ON);
	ASSERT_EQ(event.note_event.note, note);
	ASSERT_EQ(event.note_event.velocity, velocity);
}

TEST(Notes, NoteOnVelocityZero)
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 0;
	MidiNoteMessage message = { Midi::Status::NoteOnChannel1, note, velocity };

	ezmidi::MessageProcessor message_processor;
	message_processor.processMidiMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(message_processor.getNextEvent(event), 0);

	ASSERT_EQ(event.type, EZMIDI_EVENT_NOTE);
	ASSERT_EQ(event.note_event.detail, EZMIDI_NOTEEVENT_OFF);
	ASSERT_EQ(event.note_event.note, note);
	ASSERT_EQ(event.note_event.velocity, velocity);
}

TEST(Notes, NoteOnChannel)
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 50;
	MidiNoteMessage message = { Midi::Status::NoteOnChannel1, note, velocity };

	ezmidi::MessageProcessor message_processor;

	Ezmidi_Event event;
	int expected_channel = 1;

	for (int status = Midi::Status::NoteOnChannel1; status <= Midi::Status::NoteOnChannel16; status++) {
		message[0] = static_cast<uint8_t>(status);
		message_processor.processMidiMessage(message.data());
		ASSERT_NE(message_processor.getNextEvent(event), 0);

		ASSERT_EQ(event.note_event.channel, expected_channel);

		expected_channel += 1;
	}
	
}

TEST(Notes, NoteOff)
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 0;
	MidiNoteMessage message = { Midi::Status::NoteOffChannel1, note, velocity };

	ezmidi::MessageProcessor message_processor;
	message_processor.processMidiMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(message_processor.getNextEvent(event), 0);

	ASSERT_EQ(event.type, EZMIDI_EVENT_NOTE);
	ASSERT_EQ(event.note_event.detail, EZMIDI_NOTEEVENT_OFF);
	ASSERT_EQ(event.note_event.note, note);
	ASSERT_EQ(event.note_event.velocity, velocity);
}

TEST(Notes, NoteOffChannel)
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 50;
	MidiNoteMessage message = { Midi::Status::NoteOffChannel1, note, velocity };

	ezmidi::MessageProcessor message_processor;

	Ezmidi_Event event;
	int expected_channel = 1;

	for (int status = Midi::Status::NoteOffChannel1; status <= Midi::Status::NoteOffChannel16; status++) {
		message[0] = static_cast<uint8_t>(status);
		message_processor.processMidiMessage(message.data());
		ASSERT_NE(message_processor.getNextEvent(event), 0);

		ASSERT_EQ(event.note_event.channel, expected_channel);

		expected_channel += 1;
	}

}
