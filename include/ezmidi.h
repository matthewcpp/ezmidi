#ifndef EZ_MIDI_H
#define EZ_MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

struct ezmidi;

typedef enum {
	EZMIDI_NOTE,
} Ezmidi_EventType;

typedef enum {
	EZMIDI_NOTEEVENT_ON,
	EZMIDI_NOTEEVENT_OFF
} Ezmidi_NoteEventId;

typedef struct {
	Ezmidi_EventType type;
	Ezmidi_NoteEventId detail;
	int note;
	int velocity;
} Ezmidi_NoteEvent;

typedef union {
	Ezmidi_EventType type;
	Ezmidi_NoteEvent note_event;
} Ezmidi_Event;
    
typedef void(*Ezmidi_LogFunc)(const char* message);
    
typedef struct {
    Ezmidi_LogFunc log_func;
} Ezmidi_Config;

ezmidi* ezmidi_create(Ezmidi_Config* config);
void ezmidi_destroy(ezmidi* context);
void ezmidi_config_init(Ezmidi_Config* config);

int ezmidi_get_source_count(ezmidi* context);
const char* ezmidi_get_source_name(ezmidi* context, int source_index);

void ezmidi_connect_source(ezmidi* context, int source);
int ezmidi_pump_events(ezmidi* context, Ezmidi_Event* event);

#ifdef __cplusplus
}
#endif
    
#endif
