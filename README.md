# ezmidi
Ezmidi is a library for reading messages from a midi controller.  The goal of this project is to create a clean, simple and platform independant API that can easily be used standalone or as a plugin.

### Supported Platforms
1. Windows via [Multimedia API](https://docs.microsoft.com/en-us/windows/desktop/multimedia/musical-instrument-digital-interface--midi)
1. Linux via [Alsa Raw Midi API](https://www.alsa-project.org/alsa-doc/alsa-lib/rawmidi.html)
1. MacOS and iOS via [CoreMidi](https://developer.apple.com/documentation/coremidi?language=objc)
1. Unity via [ezmidi_unity](https://github.com/matthewcpp/ezmidi_unity)

## Building 

### Building on Windows, Linux, and OSX
Building for desktop platforms is very simple as there are no external dependencies.  Simply run cmake from the root directory of the repository.

### Building on iOS
The iOS build makes use of the [CMake IOS Toolchain](https://cmake.org/cmake/help/v3.14/manual/cmake-toolchains.7.html).  Users wishing to build for iOS should ensure they are using CMake version > 3.14.  The build has been tested using the XCode generator.  Ensure that when you invoke cmake to configure and generate the project you specify "iOS" for the `CMAKE_SYSTEM_NAME`.  Aside from the paramters supported by the CMake iOS toolchain, the library does not require any additional switches.

## Getting Started
Refer to the sample for your particular platform, but in general usage consists of creating a context, connecting to a source, and then pumping events.  For example:

```c++
Ezmidi_Context* ezmidi = ezmidi_create(nullptr);

if (ezmidi_get_source_count(ezmidi) == 0) {
	std::cout << "No MIDI sources found." << std::endl;
	return 1;
}

std::cout << "Connecting to source: " ezmidi_get_source_name(ezmidi, 0) << std::endl;
ezmidi_connect_source(ezmidi, 0);

while (monitor_midi_events) {
	Ezmidi_Event event;

	while (ezmidi_pump_events(ezmidi, &event)) {
		std::cout << "MIDI Event: " << event.type << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
```