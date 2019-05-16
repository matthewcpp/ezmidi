#include "ezmidi.h"
#include "event.h"
#include "midi.h"

#include <CoreMIDI/MIDIServices.h>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

// MRL 05/09/2019
// Note that there appears to be a bug when calling MIDIClientCreate from within a bundle
// The method can potentially fail with an error code of -50.
// Discussion from RtMidi issue page: https://github.com/thestk/rtmidi/issues/155
// Workaround is to open MIDIMonitor in the background before calling MIDIClientCreate from bundle: https://www.snoize.com/MIDIMonitor/

struct EzmidiCoreMidi {
	MIDIClientRef midi_client = NULL;
	MIDIPortRef midi_port = NULL;
	Ezmidi::EventQueue event_queue;
    std::string return_data;
    std::mutex mutex;
    Ezmidi_Config config;
};

void midiReadProc(const MIDIPacketList* packetList, void* refCon, void* srcConnRefCon)
{
	auto* coremidi = reinterpret_cast<EzmidiCoreMidi*>(refCon);
	
	MIDIPacket *packet = (MIDIPacket*)packetList->packet;

	for (int i = 0; i < packetList->numPackets; i++) {
        std::lock_guard<std::mutex>(coremidi->mutex);
        coremidi->event_queue.processMessage(packet->data);

		packet = MIDIPacketNext(packet);
	}
	
}

void log_error(EzmidiCoreMidi* coremidi, const char* method, OSStatus status)
{
    if (coremidi->config.log_func) {
        std::ostringstream s;
        s << method << " failed: " << status;
        coremidi->config.log_func(s.str().c_str(), coremidi->config.user_data);
    }
}

// Disposing of a midi client will also clean up all it's ports.
void close_existing_connection(EzmidiCoreMidi* coremidi)
{
    if (coremidi->midi_client) {
        if (coremidi->config.log_func) {
            coremidi->config.log_func("closing existing connection", coremidi->config.user_data);
        }
        
        OSStatus error = MIDIClientDispose(coremidi->midi_client);
        if (error)  log_error(coremidi, "MIDIClientDispose", error);
        
        coremidi->midi_port = NULL;
        coremidi->midi_client = NULL;
    }
}

Ezmidi_Context* ezmidi_create(Ezmidi_Config* config)
{
	auto* coremidi = new EzmidiCoreMidi();
    if (config)
        coremidi->config = *config;
    else
        ezmidi_config_init(&coremidi->config);
	
	return reinterpret_cast<Ezmidi_Context*>(coremidi);
}

void ezmidi_destroy(Ezmidi_Context* context)
{
	auto* coremidi = reinterpret_cast<EzmidiCoreMidi*>(context);
    {
        std::lock_guard<std::mutex>(coremidi->mutex);
        close_existing_connection(coremidi);
    }
	
	delete coremidi;
}

int ezmidi_get_source_count(Ezmidi_Context* context)
{
	return MIDIGetNumberOfSources();
}

std::string get_source_property(MIDIEndpointRef midi_source, CFStringRef property_name)
{
	CFStringRef cf_string;
	char property_buffer[64];
	MIDIObjectGetStringProperty(midi_source, property_name, &cf_string);
	CFStringGetCString(cf_string, property_buffer, sizeof(property_buffer), 0);
	CFRelease(cf_string);
	
	return std::string(property_buffer);
}

const char* ezmidi_get_source_name(Ezmidi_Context* context, int source_index)
{
	int source_count = MIDIGetNumberOfSources();
	if (source_index < 0 || source_index >= source_count)
		return nullptr;
	
	auto* coremidi = reinterpret_cast<EzmidiCoreMidi*>(context);
	MIDIEndpointRef midi_source = MIDIGetSource(source_index);
    
    std::string manufacturer = get_source_property(midi_source, kMIDIPropertyManufacturer);
	std::string model = get_source_property(midi_source, kMIDIPropertyModel);
    
    std::string result = manufacturer;
    if (!model.empty()) {
        if (!result.empty())result.append(" ");
        result.append(model);
    }
    
    if (!result.empty()) {
        coremidi->return_data = result;
    }
    else {
        coremidi->return_data = get_source_property(midi_source, kMIDIPropertyModel);
    }
	return coremidi->return_data.c_str();
}

Ezmidi_Error ezmidi_connect_source(Ezmidi_Context* context, int source)
{
	auto* coremidi = reinterpret_cast<EzmidiCoreMidi*>(context);
    
    std::lock_guard<std::mutex>(coremidi->mutex);
    if (source < 0 || source >= MIDIGetNumberOfSources()) {
        return EZMIDI_ERROR_INVALID_SOURCE;
    }
    
    close_existing_connection(coremidi);
    
    if (coremidi->config.log_func) {
        coremidi->config.log_func("opening connection", coremidi->config.user_data);
    }
    
    OSStatus error = MIDIClientCreate(CFSTR("coremidi_client"), nullptr, nullptr, &coremidi->midi_client);
    if (error) {
        log_error(coremidi, "MIDIClientCreate", error);
        MIDIClientDispose(coremidi->midi_client);
		coremidi->midi_client = NULL;
        return EZMIDI_ERROR_CONNECTION_FAILED;
    }
    
    error = MIDIInputPortCreate(coremidi->midi_client, CFSTR("coremidi_port"), midiReadProc, coremidi, &coremidi->midi_port);
    if (error) {
        log_error(coremidi, "MIDIInputPortCreate", error);
        MIDIClientDispose(coremidi->midi_client);
		coremidi->midi_client = NULL;
        
        return EZMIDI_ERROR_CONNECTION_FAILED;
    }
	
	MIDIEndpointRef midi_source = MIDIGetSource(source);
	
	if (midi_source != 0) {
		OSStatus error = MIDIPortConnectSource(coremidi->midi_port, midi_source, nullptr); // todo: handle for identifying connections?
        if (error) {
			log_error(coremidi, "MIDIPortConnectSource", error);
			MIDIClientDispose(coremidi->midi_client);
			coremidi->midi_client = NULL;
			return EZMIDI_ERROR_CONNECTION_FAILED;
		}
	}
	else {
		log_error(coremidi, "MIDIGetSource", 0);
		MIDIClientDispose(coremidi->midi_client);
		coremidi->midi_client = NULL;
		return EZMIDI_ERROR_CONNECTION_FAILED;
	}
	
	return EZMIDI_ERROR_NONE;
}

int ezmidi_pump_events(Ezmidi_Context* context, Ezmidi_Event* event)
{
	if (!event) return 0;
	
	auto* coremidi = reinterpret_cast<EzmidiCoreMidi*>(context);
	return coremidi->event_queue.pumpEvents(*event);
}
