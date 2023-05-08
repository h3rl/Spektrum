#pragma once
#include "AudioSink.h"

class Config
{
public:
	Config() {};
	Config(Config const&) = delete;
	void operator=(Config const&) = delete;

    static Config& get()
    {
        static Config instance;
        return instance;
    }

	void Save();
	void Load();

public:
	struct _Gui
	{
		bool show_fps{ true };
		bool show_menu{ true };
	} gui;

	struct _Window
	{
		int width{ 1600 };
		int height{ 900 };
		float zoom{ 1.0f };

		bool fullscreen{ false };
		bool vsync{ true };
		int max_fps = 165;

		bool in_gui{ false };
		bool need_redraw{ false };

	} window;

	struct _Audio
	{
		enum _AudioWindowFunction : int
		{
			None,
			Blackman,
			Hamming,
			//Hann,
			//Rectangle,
			//Triangle,
			//Welch,
		} windowfunction{ Blackman };

		int bar_count{ FFT_SIZE_HALF };
		float bar_gain{ 1.0f };
		float time_smoothing{ 0.5f };
		float min_db{ -120.0f };
		float max_db{ 0.0f };
		float min_freq{ 20.0f }; 
		float max_freq{ 20000.0f };

		enum _AudioBarStyle : int
		{
			Linear,
			Db,
		} barstyle{ Db };
	} audio;

	//struct _Config
	//{
	//	bool load{ false };
	//	bool save{ false };
	//}config;
};

#define CONFIG Config::get()
