#pragma once
#include "AudioSink.h"



enum AudioWindowFunction : int
{
	None,
	Blackman,
	Hamming,
	//Hann,
	//Rectangle,
	//Triangle,
	//Welch,
};

enum AudioBarStyle : int
{
	Linear,
	Db,
};

// Stuff we dont need to save
namespace state
{
	extern bool window_needs_redraw;
	extern bool in_gui;

	extern bool window_show_fps;
	extern bool window_show_menu;
	extern bool window_show_config;
}


// Stuff we need to save
namespace config
{
	extern int window_width;
	extern int window_height;
	extern unsigned int window_fps;

	namespace audio
	{
		extern AudioWindowFunction windowfunction;
		extern AudioBarStyle barstyle;
		extern int bar_count;
		extern float bar_gain;
		extern float time_smoothing;
		extern float min_db;
		extern float max_db;
		extern float min_freq;
		extern float max_freq;
	}

	void Save();
	void Load();
}