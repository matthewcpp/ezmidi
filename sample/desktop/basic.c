/*
This sample file attempts to open a connection to the MIDI source connected to the system
and outputs a set number of events before closing the connection and terminating.
*/

#include <ezmidi/ezmidi.h>

#include "common.h"

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define NumEventsToProcess 20

int main(int argc, char** argv) {
	int events_processed = 0;
	Ezmidi_Context* ezmidi = ezmidi_create(NULL);

	if (ezmidi_get_source_count(ezmidi) == 0) {
		puts("No MIDI sources available");
		ezmidi_destroy(ezmidi);
		return 1;
	}

	printf("Connecting to %s.\n", ezmidi_get_source_name(ezmidi, 0));

	Ezmidi_Error error = ezmidi_connect_source(ezmidi, 0);
	if (error) {
		puts("Connection Error.");
		ezmidi_destroy(ezmidi);
		return 1;
	}

	Ezmidi_Event event;
	while (events_processed < NumEventsToProcess) {
		while (ezmidi_get_next_event(ezmidi, &event)) {
			events_processed += 1;
			puts(event_to_string(&event));
		}

		sleep_for(1);
	}

	ezmidi_destroy(ezmidi);

	return 0;
}