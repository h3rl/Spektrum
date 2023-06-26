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
class State
{
public:
	State() {};

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
		float bar_gain{ 1.0f };
		float time_smoothing{ 0.82f };
		float min_db{ -60.0f };
		float max_db{ 0.0f };
		float min_freq{ 20.0f };
		float max_freq{ 20000.0f };

		float bass_threshold_a{ -15.f };
		float bass_threshold_b{ 1.55f };
	} audio;

	/* IO functions */
	void Save();
	void Load();
	bool LoadOnStartup();
};

extern Config config;
