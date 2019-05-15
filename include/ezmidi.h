#ifndef EZ_MIDI_H
#define EZ_MIDI_H

#ifdef _WIN32
#    ifdef BUILD_EZMIDI_DLL
#        define EZMIDI_API __declspec(dllexport)
#    else
#        define EZMIDI_API __declspec(dllimport)
#    endif
#else
#    define EZMIDI_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Ezmidi_Context Ezmidi_Context;

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
    
typedef void(*Ezmidi_LogFunc)(const char* message, void* user_data);
    
typedef struct {
    Ezmidi_LogFunc log_func;
	void* user_data;
} Ezmidi_Config;

EZMIDI_API Ezmidi_Context* ezmidi_create(Ezmidi_Config* config);
EZMIDI_API void ezmidi_destroy(Ezmidi_Context* context);
EZMIDI_API void ezmidi_config_init(Ezmidi_Config* config);

EZMIDI_API int ezmidi_get_source_count(Ezmidi_Context* context);
EZMIDI_API const char* ezmidi_get_source_name(Ezmidi_Context* context, int source_index);

EZMIDI_API void ezmidi_connect_source(Ezmidi_Context* context, int source);
EZMIDI_API int ezmidi_pump_events(Ezmidi_Context* context, Ezmidi_Event* event);

#ifdef __cplusplus
}
#endif
    
#endif
