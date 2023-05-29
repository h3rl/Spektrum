#pragma once
#include "AudioSink.h"


#define AudioWindowFunctionCombo "None\0Blackman\0Hamming\0Hann\0Rect\0Triangle\0Welch\0FlatTop\0BlackmanHarris\0\0"
enum AudioWindowFunction : int
{
	None,
	Blackman,
	Hamming,
	Hann,
	Rect,
	Triangle,
	Welch,
	FlatTop,
	BlackmanHarris
};

enum AudioAxisScale : int
{
	Linear,
	Logarithmic,
};

// Stuff we dont need to save
namespace state
{
	extern bool window_needs_redraw;
	extern bool in_gui;

	extern bool window_show_fps;
	extern bool window_show_menu;
	extern bool window_show_config;

	extern int audio_samplerate;

	extern std::vector<std::string> debug_textvec;
}


// Stuff we need to save
namespace config
{
	extern int window_width;
	extern int window_height;

	extern bool load_config_on_startup;

	namespace graphics
	{
		extern bool vsync;
		extern int framerate_limit;
		extern int antialiasing_level;
	}

	namespace audio
	{
		extern AudioWindowFunction windowfunction;
		extern AudioAxisScale barstyle;
		extern int bar_count;
		extern float bar_gain;
		extern float time_smoothing;
		extern float min_db;
		extern float max_db;
		extern float min_freq;
		extern float max_freq;

		extern float bass_threshold_a;
		extern float bass_threshold_b;
	}

	void Save();
	void Load();
	bool LoadOnStartup();
}