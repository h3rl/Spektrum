#include "stdafx.h"

#include "Gui.h"

Gui::Gui()
{
}

Gui::~Gui()
{
	ImGui::SFML::Shutdown();
}

void Gui::preupdate()
{
	state.debug_textvec.clear();
}

bool Gui::init()
{
	if (!ImGui::SFML::Init(g_window))
	{
		_D("GUI: Imgui init failed");
		return false;
	}

	if (!m_font.loadFromFile("Assets\\arial.ttf"))
	{
		_D("GUI: Load font failed");
		MessageBoxA(NULL, "Font could not be loaded\nat \'./Assets/arial.ttf\'", "Error", MB_OK);
		return false;
	}

	m_fpsText.setFont(m_font);
	m_fpsText.setString("");
	m_fpsText.setCharacterSize(14); // in pixels, not points!
	m_fpsText.setFillColor(sf::Color(110, 255, 0));
	m_fpsText.setPosition({ 0,0 });

	return true;
}

void Gui::update(const sf::Time& dtTime)
{
	ImGui::SFML::Update(g_window,dtTime);

	state.in_gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

	m_fpsText.setString(std::to_string((int)( 1 / dtTime.asSeconds())));
}

void Gui::render()
{
	renderMenuV1();

	//ImGui::ShowDemoWindow();
	ImGui::SFML::Render(g_window);

	if (state.window_show_fps)
	{
		renderFpsText();
	}
}

void Gui::renderFpsText()
{
	sf::View oldview = g_window.getView();
	sf::View view = g_window.getSizedView();
	// Reset view to default, so we can draw the text
	g_window.setView(view);
	g_window.draw(m_fpsText);

	// Set it back to the original view
	g_window.setView(oldview);
}

void Gui::renderMenuV1()
{	
	if (!ImGui::Begin("GUI", &state.window_show_menu, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos({ 10, 10 }, ImGuiCond_Once);

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

void Gui::renderTabMain()
{
	ImGui::Combo("Bar style", (int*)&config.audio.barstyle, "Linear\0Logarithmic\0\0");

	if (ImGui::SliderInt("Bar Count", &config.audio.bar_count, 1, FFT_SIZE_HALF, "%d"))
	{
		state.window_needs_redraw = true;
	}
	ImGui::InputFloat("Bar Gain", &config.audio.bar_gain, 0.1f, 1.0f, "%.2f");


	if (ImGui::InputFloat("Min Db", &config.audio.min_db, 0.1f, 1.0f, "%.2f"))
	{
		config.audio.min_db = clamp(config.audio.min_db, -std::numeric_limits<float>::infinity(), config.audio.max_db);
	}
	if (ImGui::InputFloat("Max Db", &config.audio.max_db, 0.1f, 1.0f, "%.2f"))
	{
		config.audio.max_db = clamp(config.audio.max_db, config.audio.min_db, 25.0f);
	}

	if (ImGui::InputFloat("Time Smoothing", &config.audio.time_smoothing, 0.1f, 1.0f, "%.3f"))
	{
		config.audio.time_smoothing = std::max(config.audio.time_smoothing, 0.f);
	}

	ImGui::Combo("FFT Windowingfunction", (int*)&config.audio.windowfunction, AudioWindowFunctionCombo);

	if (ImGui::InputFloat("Min bass freq", &config.audio.min_freq, 10.f, 100.f, "%.2f"))
	{
		config.audio.min_freq = clamp(config.audio.min_freq, 0, config.audio.max_freq);
	}
	if (ImGui::InputFloat("Max bass freq", &config.audio.max_freq, 10.f, 100.f, "%.2f"))
	{
		config.audio.max_freq = clamp(config.audio.max_freq, config.audio.min_freq, std::numeric_limits<float>::infinity());
	}

	ImGui::InputFloat("BassCoeffA", &config.audio.bass_threshold_a, 0.1f, 1.f, "%.4f");
	ImGui::InputFloat("BassCoeffB", &config.audio.bass_threshold_b, 0.1f, 1.f, "%.4f");

}
void Gui::renderTabConfig()
{
	if (ImGui::Button("Load config"))
	{
		config.Load();
	}
	if (ImGui::Button("Save config"))
	{
		config.Save();
	}

	ImGui::Checkbox("Load config at startup", &config.load_config_on_startup);

}
void Gui::renderTabGraphics()
{
	if (ImGui::Checkbox("Enable VSync (not recommended)", &config.graphics.vsync))
	{
		g_window.setVerticalSyncEnabled(config.graphics.vsync);
	}

	if (!config.graphics.vsync)
	{
		if (ImGui::InputInt("FPS Limit", &config.graphics.framerate_limit, 0, 10))
		{
			g_window.setFramerateLimit(config.graphics.framerate_limit);
		}
	}
}

void Gui::renderTabDebug()
{
	for (const auto& val : state.debug_textvec)
	{
		ImGui::Text(val.c_str());
	}
}

Gui g_gui;