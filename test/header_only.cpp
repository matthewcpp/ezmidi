/*
	This file simply checks that the header only version of the library compiles and links correctly.
*/

#define EZMIDI_IMPLEMENTATION
#include "ezmidi/ezmidi.hpp"

int main(int argc, char** argv) {
	Ezmidi_Context* ezmidi = ezmidi_create(nullptr);
	ezmidi_destroy(ezmidi);
	return 0;
}