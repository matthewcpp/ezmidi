#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char message_buffer[128];

char* event_to_string(Ezmidi_Event* event)
{
	switch (event->type) {
	case EZMIDI_EVENT_NOTE: {
		const char* detail = event->note_event.detail == EZMIDI_NOTEEVENT_ON ? "On" : "Off";
		snprintf(message_buffer, sizeof(message_buffer), "Note %s Event: Note: %d Velocity: %d Channel: %d",
			detail, event->note_event.note, event->note_event.velocity, event->note_event.channel);
		break;
	}
	default:
		snprintf(message_buffer, sizeof(message_buffer), "Unknown event type: %d\n", event->type);
	}

	return message_buffer;
}


#ifdef _WIN32
#include <Windows.h>

void sleep_for(int milliseconds) 
{
	Sleep(milliseconds);
}

#else
#include <time.h>

void sleep_for(int milliseconds)
{
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

#endif

