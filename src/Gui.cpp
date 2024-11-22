#include "gui.h"
#include "config.h"
#include "logging.h"
#include "window.h"
#include "audio_sink.h"
#include "beat_detector.h"
#include "dft.h"

#include <imgui.h>
#include <imgui-SFML.h>

namespace Gui
{

// forward declarations
void renderFpsText();
void renderMenuV1();
void renderTabMain();
void renderTabConfig();
void renderTabGraphics();
void renderTabDebug();

// Globals
bool hovered = false;
bool show_fps = true;
bool show_gui = true;

sf::Font font;
sf::Text fps_text;

void release()
{
	ImGui::SFML::Shutdown();
}

void preupdate()
{
	debug_vec.clear();
}

void init()
{
	bool imgui_init = ImGui::SFML::Init(*p_window);
	assert_msg(imgui_init, "GUI: Imgui init failed");

	ImGui::GetIO().IniFilename = nullptr;

	bool font_loaded = font.loadFromFile("assets\\arial.ttf");
	if (!font_loaded)
	{
		// Try to load from windows font folder
		font_loaded = font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
	}
	assert_msg(font_loaded, "GUI: Load font failed");

	fps_text.setFont(font);
	fps_text.setString("");
	fps_text.setCharacterSize(14); // in pixels, not points!
	fps_text.setFillColor(sf::Color(110, 255, 0));
	fps_text.setPosition({ 0, 0 });
}

void update(const sf::Time &dtTime)
{
	ImGui::SFML::Update(*p_window, dtTime);

	hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

	fps_text.setString(std::to_string((int)(1 / dtTime.asSeconds())));
}

void render()
{
	renderMenuV1();

	//ImGui::ShowDemoWindow();
	ImGui::SFML::Render(*p_window);

	if (show_fps)
	{
		renderFpsText();
	}
}

void renderFpsText()
{
	// sf::View oldview = p_window->getView();
	// sf::View newview = p_window->getDefaultView();
	// Reset view to default, so we can draw the text
	// p_window->setView(newview);
	p_window->setView(p_window->getDefaultView());
	p_window->draw(fps_text);

	// Set it back to the original view
	// p_window->setView(oldview);
}

void renderMenuV1()
{
	if (!ImGui::Begin("GUI", &show_gui, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos({ 10, 10 }, ImGuiCond_Once);

	ImGui::Text("Test");

	if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Menu"))
		{
			// MAIN START
			renderTabMain();
			// MAIN END

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Graphics"))
		{
			// GRAPHICS START
			renderTabGraphics();
			// GRAPHICS END

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Config"))
		{
			// CONFIG START
			renderTabConfig();
			// CONFIG END

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Debug"))
		{
			// DEBUG START
			renderTabDebug();
			// DEBUG END

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void renderTabMain()
{
	ImGui::Text("Bars style");
	ImGui::InputInt("DFT Size", (int *)&BeatDetector::dft_size, 1, (int)MAX_DFT_SIZE);
	{
		Window::needs_redraw = true;
	}
	ImGui::SliderFloat("DFT ZeroPad Factor", &BeatDetector::dft_zeropad_factor, 0.0f, 1.0f);

	ImGui::SliderFloat("Filter Factor", &BeatDetector::filter_factor_sigma, 0.f, 100.0f);

	ImGui::SliderFloat("Time Smoothing", &BeatDetector::time_smoothing, 0.f, 1.0f);

	ImGui::Text("Bar Positioning Y");
	ImGui::DragFloat("Min Db", &BeatDetector::db_min, 5.0f);
	ImGui::DragFloat("Max Db", &BeatDetector::db_max, 5.0f);

	ImGui::Text("Bar Positioning X");
	ImGui::InputFloat("Min Freq", &BeatDetector::f_min, 10.0f);
	ImGui::InputFloat("Max Freq", &BeatDetector::f_max, 10.0f);

	ImGui::Text("Loudness / Beatdetection");
	ImGui::InputFloat("Loudness Alpha", &BeatDetector::loudness_alpha, 0.01f);
	ImGui::InputFloat("Loudness Min Freq", &BeatDetector::loudness_f_min, 10.0f);
	ImGui::InputFloat("Loudness Max Freq", &BeatDetector::loudness_f_max, 10.0f);

	// ImGui::Combo("Bar style", (int *)&config.audio.barstyle, "Linear\0Logarithmic\0\0");

	// if (ImGui::SliderInt("Bar Count", &config.audio.bar_count, 1, FFT_SIZE_HALF, "%d"))
	// {
	// 	state.window_needs_redraw = true;
	// }
	// ImGui::Inputdouble("Bar Gain", &config.audio.bar_gain, 0.1f, 1.0f, "%.2f");

	// if (ImGui::Inputdouble("Min Db", &config.audio.min_db, 0.1f, 1.0f, "%.2f"))
	// {
	// 	config.audio.min_db = clamp(config.audio.min_db, -std::numeric_limits<float>::infinity(), config.audio.max_db);
	// }
	// if (ImGui::Inputdouble("Max Db", &config.audio.max_db, 0.1f, 1.0f, "%.2f"))
	// {
	// 	config.audio.max_db = clamp(config.audio.max_db, config.audio.min_db, 25.0f);
	// }

	// if (ImGui::Inputdouble("Time Smoothing", &config.audio.time_smoothing, 0.1f, 1.0f, "%.3f"))
	// {
	// 	config.audio.time_smoothing = std::max(config.audio.time_smoothing, 0.f);
	// }

	// ImGui::Combo("DFT Windowingfunction", (int *)&config.audio.windowfunction, AudioWindowFunctionCombo);

	// if (ImGui::Inputdouble("Min bass freq", &config.audio.min_freq, 10.f, 100.f, "%.2f"))
	// {
	// 	config.audio.min_freq = clamp(config.audio.min_freq, 0, config.audio.max_freq);
	// }
	// if (ImGui::Inputdouble("Max bass freq", &config.audio.max_freq, 10.f, 100.f, "%.2f"))
	// {
	// 	config.audio.max_freq =
	// 	    clamp(config.audio.max_freq, config.audio.min_freq, std::numeric_limits<float>::infinity());
	// }

	// ImGui::Inputdouble("BassCoeffA", &config.audio.bass_threshold_a, 0.1f, 1.f, "%.4f");
	// ImGui::Inputdouble("BassCoeffB", &config.audio.bass_threshold_b, 0.1f, 1.f, "%.4f");
}
void renderTabConfig()
{
	if (ImGui::Button("Load config"))
	{
		Config::Load();
	}
	if (ImGui::Button("Save config"))
	{
		Config::Save();
	}
}
void renderTabGraphics()
{
	if (ImGui::Checkbox("Enable VSync (not recommended)", &Window::vsync))
	{
		p_window->setVerticalSyncEnabled(Window::vsync);
	}

	if (!Window::vsync)
	{
		if (ImGui::InputInt("FPS Limit", &(int &)Window::framerate_limit, 0, 10))
		{
			p_window->setFramerateLimit(Window::framerate_limit);
		}
	}
}

void renderTabDebug()
{
	 for (const auto &val : debug_vec)
 {
		ImGui::Text(val.c_str());
	}
}
} // namespace Gui