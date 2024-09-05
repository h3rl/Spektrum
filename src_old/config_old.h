#pragma once
#include "audio_sink.h"

//#include <libconfig.h++>
//
//static libconfig::Config cfg;
//using libconfig::FileIOException;
//using libconfig::ParseException;

//void LoadConfig()
//{
//	try
//	{
//		cfg.readFile("config.cfg");
//	}
//	catch (const FileIOException &fioex)
//	{
//		std::cerr << "I/O error while reading file." << std::endl;
//		exit(EXIT_FAILURE);
//		return;
//	}
//	catch (const ParseException &pex)
//	{
//		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
//		exit(EXIT_FAILURE);
//		return;
//	}
//}

enum AudioAxisScale : int
{
	Linear,
	Logarithmic,
};

// Stuff we dont need to save
class State
{
public:
	State(){};

	bool window_needs_redraw{ false };
	bool in_gui{ false };

	bool window_show_fps{ true };
	bool window_show_menu{ true };
	bool window_show_config{ false };

	int audio_samplerate{ 0 };

	std::vector<std::string> debug_textvec{};
};

extern State state;

// Stuff we need to save
class Config
{
public:
	/* Fields */
	int window_width{ 1600 };
	int window_height{ 900 };
	bool load_config_on_startup{ true };

	struct _Graphics
	{
		bool vsync{ false };
		int framerate_limit{ 240 };
		int antialiasing_level{ 4 };
	} graphics;

	struct _Audio
	{
		AudioWindowFunction windowfunction{ Blackman };
		AudioAxisScale barstyle{ Logarithmic };
		int bar_count{ (int)(FFT_SIZE_HALF) };
		double bar_gain{ 1.0f };
		double time_smoothing{ 0.82f };
		double min_db{ -60.0f };
		double max_db{ 0.0f };
		double min_freq{ 20.0f };
		double max_freq{ 20000.0f };

		double bass_threshold_a{ -15.f };
		double bass_threshold_b{ 1.55f };
	} audio;

	/* IO functions */
	void Save();
	void Load();
	bool LoadOnStartup();
};

extern Config config;
