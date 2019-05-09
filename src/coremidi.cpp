#include "ezmidi.h"
#include "event.h"
#include "midi.h"

#include <CoreMIDI/MIDIServices.h>
#include <iostream>
#include <any>
#include <string>
#include <sstream>

struct ez_coremidi {
	MIDIClientRef midi_client = NULL;
	MIDIPortRef midi_port = NULL;
	Ezmidi::EventQueue event_queue;
	std::any return_data;
};

void midiReadProc(const MIDIPacketList* packetList, void* refCon, void* srcConnRefCon)
{
	auto* coremidi = reinterpret_cast<ez_coremidi*>(refCon);
	
	MIDIPacket *packet = (MIDIPacket*)packetList->packet;

	for (int i = 0; i < packetList->numPackets; i++) {
		int midiStatus = packet->data[0];
		
		if (Midi::shouldFilterEvent(midiStatus)) {
			continue;
		}
		else if (Midi::isNoteEvent(midiStatus)) {
			int noteNumber = packet->data[1];
			int velocity = packet->data[2];
			coremidi->event_queue.pushNote(midiStatus, noteNumber, velocity);
		}

		packet = MIDIPacketNext(packet);
	}
	
}

ezmidi* ezmidi_create()
{
	auto* coremidi = new ez_coremidi();
	
	OSStatus error = MIDIClientCreate(CFSTR("client"), nullptr, nullptr, &coremidi->midi_client);
	if (error) {
		std::cout << "Error creating client" << error << std::endl;
		ezmidi_destroy(reinterpret_cast<ezmidi*>(coremidi));
		
		return NULL;
	}
	
	error = MIDIInputPortCreate(coremidi->midi_client, CFSTR("Input port"), midiReadProc, coremidi, &coremidi->midi_port);
	
	if (error) {
		std::cout << "Error creating Port" << error << std::endl;
		ezmidi_destroy(reinterpret_cast<ezmidi*>(coremidi));
		
		return NULL;
	}
	
	return reinterpret_cast<ezmidi*>(coremidi);
}

void ezmidi_destroy(ezmidi* context)
{
	auto* coremidi = reinterpret_cast<ez_coremidi*>(context);
	
	if (coremidi->midi_port){
		MIDIPortDispose(coremidi->midi_port);
	}
	
	if (coremidi->midi_client) {
		MIDIClientDispose(coremidi->midi_client);
	}
	
	delete coremidi;
}

int ezmidi_get_source_count(ezmidi* context)
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

const char* ezmidi_get_source_name(ezmidi* context, int source_index)
{
	int source_count = MIDIGetNumberOfSources();
	if (source_index < 0 || source_index >= source_count)
		return nullptr;
	
	auto* coremidi = reinterpret_cast<ez_coremidi*>(context);
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
	return std::any_cast<std::string&>(coremidi->return_data).c_str();
}

void ezmidi_connect_source(ezmidi* context, int source)
{
    if (source < 0 || source >= MIDIGetNumberOfSources()) {
        return;
    }
    
	auto* coremidi = reinterpret_cast<ez_coremidi*>(context);
	
	MIDIEndpointRef midi_source = MIDIGetSource(source);
	
	if (midi_source != 0) {
		MIDIPortConnectSource(coremidi->midi_port, midi_source, nullptr); // todo: handle for identifying connections?
	}
	else {
		std::cout << "Error connecting source" << std::endl;
	}
}

int ezmidi_pump_events(ezmidi* context, Ezmidi_Event* event)
{
	if (!event) return 0;
	
	auto* coremidi = reinterpret_cast<ez_coremidi*>(context);
	return coremidi->event_queue.pumpEvents(*event);
}
