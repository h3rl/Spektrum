#include "stdafx.h"

#include "Gui.h"

Gui::Gui()
{
}

Gui::~Gui()
{
	ImGui::SFML::Shutdown();
}

bool Gui::init(shared_ptr<Window> window)
{
	if (!window)
	{
		_D("GUI: Window is nullptr");
		return false;
	}
	this->m_window = window;
	if (!ImGui::SFML::Init(*window))
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

void Gui::update(sf::Time dtTime)
{
	ImGui::SFML::Update(*m_window,dtTime);

	state::in_gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

	m_fpsText.setString(std::to_string((int)( 1 / dtTime.asSeconds())));
}

void Gui::render()
{
	renderMenuV1();

	//ImGui::ShowDemoWindow();
	ImGui::SFML::Render(*m_window);

	if (state::window_show_fps)
	{
		renderFpsText();
	}
}

void Gui::renderFpsText()
{
	sf::View oldview = m_window->getView();
	sf::View view = m_window->getSizedView();
	// Reset view to default, so we can draw the text
	m_window->setView(view);
	m_window->draw(m_fpsText);

	// Set it back to the original view
	m_window->setView(oldview);
}

void Gui::renderMenuV1()
{	
	if (!ImGui::Begin("GUI", &state::window_show_menu, ImGuiWindowFlags_AlwaysAutoResize))
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
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Gui::renderTabMain()
{
	ImGui::Combo("Bar style", (int*)&config::audio::barstyle, "Linear\0Logarithmic\0\0");

	if (ImGui::SliderInt("Bar Count", &config::audio::bar_count, 1, FFT_SIZE_HALF, "%d"))
	{
		state::window_needs_redraw = true;
	}
	ImGui::InputFloat("Bar Gain", &config::audio::bar_gain, 0.1f, 1.0f, "%.2f");


	if (ImGui::InputFloat("Minimum Db", &config::audio::min_db, -100.f, config::audio::max_db, "%.2f"))
	{
		config::audio::min_db = clamp(config::audio::min_db, -std::numeric_limits<float>::infinity(), config::audio::max_db);
	}
	if (ImGui::InputFloat("Maximum Db", &config::audio::max_db, config::audio::min_db, 25.f, "%.2f"))
	{
		config::audio::max_db = clamp(config::audio::max_db, config::audio::min_db, 25.0f);
	}

	ImGui::SliderFloat("Time Smoothing", &config::audio::time_smoothing, 0.f, 1.0f, "%.2f");

	ImGui::Combo("FFT Windowingfunction", (int*)&config::audio::windowfunction, AudioWindowFunctionCombo);
}
void Gui::renderTabConfig()
{
	if (ImGui::Button("Load config"))
	{
		config::Load();
	}
	if (ImGui::Button("Save config"))
	{
		config::Save();
	}

	ImGui::Checkbox("Load config at startup", &config::load_config_on_startup);

}
void Gui::renderTabGraphics()
{
	if (ImGui::Checkbox("Enable VSync (not recommended)", &config::graphics::vsync))
	{
		m_window->setVerticalSyncEnabled(config::graphics::vsync);
	}

	if (!config::graphics::vsync)
	{
		if (ImGui::InputInt("FPS Limit", &config::graphics::framerate_limit, 0, 10))
		{
			m_window->setFramerateLimit(config::graphics::framerate_limit);
		}
	}
}