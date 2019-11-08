# ezmidi
[![Build Status](https://travis-ci.org/matthewcpp/ezmidi.svg?branch=master)](https://travis-ci.org/matthewcpp/ezmidi) [![Build status](https://ci.appveyor.com/api/projects/status/tvrc7il7s82alfgo/branch/master?svg=true)](https://ci.appveyor.com/project/matthewcpp/ezmidi/branch/master) [![Documentation Status](https://readthedocs.org/projects/ezmidi/badge/?version=latest)](https://ezmidi.readthedocs.io/en/latest/?badge=latest)


Ezmidi is a library for reading messages from a midi controller.  The goal of this project is to create a clean, simple and platform independant API that can easily be used standalone or as a plugin.

### Supported Platforms
1. Windows via [Multimedia API](https://docs.microsoft.com/en-us/windows/desktop/multimedia/musical-instrument-digital-interface--midi)
1. Linux via [Alsa Raw Midi API](https://www.alsa-project.org/alsa-doc/alsa-lib/rawmidi.html)
1. MacOS and iOS via [CoreMidi](https://developer.apple.com/documentation/coremidi?language=objc)
1. Unity via [EzmidiUnity](https://github.com/matthewcpp/EzmidiUnity)

## Building 

### Dependencies
CMake 3.14 is reccomended to build ezmidi.  If you are not interested in building iOS you can use CMake 3.11.  
The [Google Test](https://github.com/google/googletest) library is also required, and will be fetched automatically by CMake during project generation.  
On linux, you will need development files for alsa.  For ubuntu, these are contained in the `libasound2-dev` package.

### Building on Windows, Linux, and OSX
The library should build out of the box without any specific CMake configuration.

### Building on iOS
The iOS build makes use of the [CMake IOS Toolchain](https://cmake.org/cmake/help/v3.14/manual/cmake-toolchains.7.html).  The build has been tested using the XCode generator.  Ensure that when you invoke cmake to configure and generate the project you specify `-DCMAKE_SYSTEM_NAME=iOS`.  Aside from the paramters supported by the CMake iOS toolchain, the library does not require any additional switches.

## Getting Started
Browse the [Public API Reference](https://ezmidi.readthedocs.io/en/latest/ezmidi_8h.html).

### Sample Usage
Refer to the sample for your particular platform, but in general usage consists of creating a context, connecting to a source, and then pumping events.  For example:

```c++
#include <ezmidi/ezmidi.h>

Ezmidi_Context* ezmidi = ezmidi_create(nullptr);

if (ezmidi_get_source_count(ezmidi) == 0) {
	std::cout << "No MIDI sources found." << std::endl;
	return 1;
}

std::cout << "Connecting to source: " ezmidi_get_source_name(ezmidi, 0) << std::endl;
ezmidi_connect_source(ezmidi, 0);

while (true) {
	Ezmidi_Event event;

	while (ezmidi_get_next_event(ezmidi, &event)) {
		std::cout << "MIDI Event: " << event.type << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

ezmidi_destroy(ezmidi)
```

### Thread Safety
The ezmidi API is not thread safe.  All public API methods with a given context should be invoked from the same thread that called `ezmidi_create`.