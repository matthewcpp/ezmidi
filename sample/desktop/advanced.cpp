/*
The advanced sample shows how to connect and disconnect from MIDI sources in a single session. 
*/
#include "common.h"

#include <ezmidi/ezmidi.h>

#include <thread>
#include <chrono>
#include <list>
#include <string>
#include <atomic>
#include <iostream>

class EzmidiDemo
{
public :
	EzmidiDemo()
	{
		Ezmidi_Config config;
		ezmidi_config_init(&config);
		config.log_func = EzmidiDemo::logMessage;
		config.user_data = this;
		is_connected_ = false;
		is_running_ = true;

		context_ = ezmidi_create(&config);
	}

	~EzmidiDemo()
	{
		if (input_thread_.joinable()) {
			input_thread_.join();
		}

		ezmidi_destroy(context_);
	}

	void update()
	{
		if (!is_connected_) {
			ezmidi_disconnect_source(context_);
			connect();
		}

        if (is_running_){
            getEvents();
        }
	}

	inline bool is_running() const { return is_running_; }

private:
	void connect()
	{
		int source_count = ezmidi_get_source_count(context_);

		if (source_count == 0) {
			std::cout << "No MIDI sources detected." << std::endl;
			is_running_ = false;
			return;
		}

		while (true) {
			std::string command;
			std::cout << "Connected MIDI Sources: " << std::endl;

			for (int i = 0; i < source_count; i++) {
				std::cout << i << ": " << ezmidi_get_source_name(context_, i) << std::endl;
			}

			std::cout << "[Index] to connect [Q] to Quit: " << std::endl;
			std::cout << "> ";
			std::getline(std::cin, command);

			if (command == "q" || command == "Q") {
				is_running_ = false;
				break;
			}
			else {
				try {
					int source = std::stoi(command);

					if (source < 0 || source > source_count) {
						throw std::invalid_argument("source index out of range");
					}

					ezmidi_connect_source(context_, source);
					std::cout << "Monitoring MIDI Events." << std::endl;
					std::cout << "Press enter to disconnect" << std::endl;
					is_connected_ = true;

					if (input_thread_.joinable()) {
						input_thread_.join();
					}

					input_thread_ = std::thread(EzmidiDemo::inputThread, this);
					break;
				}
				catch (std::invalid_argument e) { 
					continue;
				}
			}
		}
	}
    
    void getEvents()
    {
        Ezmidi_Event event;
        
        while (ezmidi_get_next_event(context_, &event)) {
            std::cout << event_to_string(&event) << std::endl;
        }
    }
    
	void disconnect()
	{
		is_connected_ = false;
	}

	static void logMessage(const char* message, void* user_data)
	{
		EzmidiDemo* ezmidi_demo = reinterpret_cast<EzmidiDemo*>(user_data);
		std::cout << "Log Message: " << message << std::endl;
    }

	static void inputThread(EzmidiDemo* demo) {
		std::cin.get();
		demo->disconnect();
	}

	Ezmidi_Context* context_;
	std::atomic_bool is_running_;
	std::atomic_bool is_connected_;
	std::thread input_thread_;
};


int main(int argc, char** argv) 
{
	EzmidiDemo demo;

	while (demo.is_running()) {
		demo.update();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}
