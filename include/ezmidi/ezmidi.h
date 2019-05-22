/**
\file ezmidi.h
Header file for ezmidi public API
*/

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

/** Enumerates error messages returned by the library. */
typedef enum {
	/** No error.  The operation was successful. */
	EZMIDI_ERROR_NONE = 0,

	/** The supplied source index is invalid.  */
	EZMIDI_ERROR_INVALID_SOURCE,

	/** The source was unable to be opened.  The device may be busy, or has possibly been disconnected. */
	EZMIDI_ERROR_CONNECTION_FAILED,

	/** The operation faild because there was no active connection. */
	EZMIDI_ERROR_NO_SOURCE_CONNECTED
} Ezmidi_Error;

/** Enumerates the type of all MIDI events. */
typedef enum {
    /** Midi note on/off event */
	EZMIDI_EVENT_NOTE,
} Ezmidi_EventType;

/** Enumerates the specific types of note events. */
typedef enum {
	/** A note is now on. */
	EZMIDI_NOTEEVENT_ON,

	/** A note is now off.  Note: if the controller sends Note on event with velocity of 0, the library will expose it as a note off event. */
	EZMIDI_NOTEEVENT_OFF
} Ezmidi_NoteEventId;

/** Structure representing a MIDI note event. */
typedef struct {
	/** The event type.  This will be EZMIDI_EVENT_NOTE */
	Ezmidi_EventType type;

	/** Indicates the type of note event: On / OFF. */
	Ezmidi_NoteEventId detail;

	/** The MIDI note number. */
	int note;

	/** The note's velocity value. */
	int velocity;

	/** the note's channel. */
	int channel;
} Ezmidi_NoteEvent;

/**
Union consisting of all possible midi event types the library can respond to.
*/
typedef union {
	Ezmidi_EventType type;
	Ezmidi_NoteEvent note_event;
} Ezmidi_Event;
    
/** Log function callback.
\param message informational message generated by the library
\param user_data user supplied callback data
*/
typedef void(*Ezmidi_LogFunc)(const char* message, void* user_data);
   
/**
Structure containing configuration settings for a ezmidi context
*/
typedef struct {
	/** Callback function for log messages */
    Ezmidi_LogFunc log_func;

	/** User data that will be passed to all callback functions. */
	void* user_data;
} Ezmidi_Config;

/**
Creates a new ezmidi context.
\param config pointer to a configuration object that will control various library settings.  If this parameter is null default configuration options will be used.
\returns pointer to a new context.  In the event of an error, this function will return null.
*/
EZMIDI_API Ezmidi_Context* ezmidi_create(Ezmidi_Config* config);

/**
Closes any active connection held by the context and frees all associated system resources.
\param context Ezmidi context pointer.
*/
EZMIDI_API void ezmidi_destroy(Ezmidi_Context* context);

/**
Initializes a config object with default values.
\param config Configuration object which will be initialized with default values.
*/
EZMIDI_API void ezmidi_config_init(Ezmidi_Config* config);

/**
Gets the number of MIDI input sources that are connected to the system.
Note: all sources may not be able be connected to.  For example a device may be in use by another application.
\param context Ezmidi context pointer.
\returns Number of MIDI input sources connected to the system.
*/
EZMIDI_API int ezmidi_get_source_count(Ezmidi_Context* context);

/**
Gets the name of an imput source.  The exact name of the device returned by this function may vary depending on platform.
In the event that this function returns a string, its memory is owned by the library and does not need to be freed by the caller.
This pointer, however, is only valid until the next ezmidi API call.  If this data needs to be persisted, it should be copied into your own data structure.
\param context Ezmidi context pointer.
\param source Index of the source to get the name of.
\returns Source name string if source is a valid index.  If source is invalid or a name could not be retrieved, this function will return null.
*/
EZMIDI_API const char* ezmidi_get_source_name(Ezmidi_Context* context, int source);

/**
Connects to the midi input source with a given index.
\param context Ezmidi context pointer.
\param source index of the source to connect to.
*/
EZMIDI_API Ezmidi_Error ezmidi_connect_source(Ezmidi_Context* context, int source);

/**
Closes the active connection for the supplied context.
\param context Ezmidi context pointer.
\returns Error code indicating the result of the operation.
*/
EZMIDI_API Ezmidi_Error ezmidi_disconnect_source(Ezmidi_Context* context);

/**
Gets whether a context has a connected source.
\param context Ezmidi context pointer.
\returns non zero value if a source is connected.  If no source is connected 0 is returned.
*/
EZMIDI_API int ezmidi_has_source_connected(Ezmidi_Context* context);

/**
Pumps the event queue for the next pending event.  Events are processed in a FIFO manner.
\param context Ezmidi context pointer.
\param event Pointer to an event object that will receive event data for next pending event.
\returns Non zero value if an event was processed, otherwise zero.
*/
EZMIDI_API int ezmidi_get_next_event(Ezmidi_Context* context, Ezmidi_Event* event);

#ifdef __cplusplus
}
#endif
    
#endif
