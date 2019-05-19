#include "ezmidi/ezmidi.h"
#include "ezmidi/private/midi.h"
#include "ezmidi/private/event.h"

#include "gtest/gtest.h"

#include <array>

using MidiNoteMessage = std::array<uint8_t, 3>;

TEST(Notes, NoteOn) 
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 50;
	MidiNoteMessage message = { Midi::Status::NoteOnChannel1, note, velocity };

	Ezmidi::EventQueue event_queue;
	event_queue.processMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(event_queue.pumpEvents(event), 0);

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

	Ezmidi::EventQueue event_queue;
	event_queue.processMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(event_queue.pumpEvents(event), 0);

	ASSERT_EQ(event.type, EZMIDI_EVENT_NOTE);
	ASSERT_EQ(event.note_event.detail, EZMIDI_NOTEEVENT_OFF);
	ASSERT_EQ(event.note_event.note, note);
	ASSERT_EQ(event.note_event.velocity, velocity);
}

TEST(Notes, NoteOff)
{
	uint8_t note = 60; // middle C
	uint8_t velocity = 0;
	MidiNoteMessage message = { Midi::Status::NoteOffChannel1, note, velocity };

	Ezmidi::EventQueue event_queue;
	event_queue.processMessage(message.data());

	Ezmidi_Event event;
	ASSERT_NE(event_queue.pumpEvents(event), 0);

	ASSERT_EQ(event.type, EZMIDI_EVENT_NOTE);
	ASSERT_EQ(event.note_event.detail, EZMIDI_NOTEEVENT_OFF);
	ASSERT_EQ(event.note_event.note, note);
	ASSERT_EQ(event.note_event.velocity, velocity);
}