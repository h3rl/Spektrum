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

	CONFIG.window.in_gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

	m_fpsText.setString(std::to_string((int)( 1 / dtTime.asSeconds())));
}

void Gui::render()
{
	//renderTestWindow();
	renderMenuV1();
	renderConfigWindow();

	//ImGui::ShowDemoWindow();
	ImGui::SFML::Render(*m_window);

	if (CONFIG.gui.show_fps)
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
	if (!ImGui::Begin("Menu", &CONFIG.gui.show_menu, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos({ 10, 100 }, ImGuiCond_Once);

	ImGui::Combo("Bar style", (int*)&CONFIG.audio.barstyle, "Magnitude\0Db\0\0");

	if (ImGui::SliderInt("Bar Count", &CONFIG.audio.bar_count, 1, FFT_SIZE_HALF, "%d"))
	{
		CONFIG.window.need_redraw = true;
	}

	ImGui::SliderFloat("TimeSmoothing", &CONFIG.audio.time_smoothing, 0.f, 1.0f, "%.2f");

	ImGui::SliderFloat("Min Db", &CONFIG.audio.min_db, -300.f, CONFIG.audio.max_db, "%.2f");
	ImGui::SliderFloat("Max Db", &CONFIG.audio.max_db, CONFIG.audio.min_db, 0.f, "%.2f");

	ImGui::InputFloat("Bar Gain", &CONFIG.audio.bar_gain, 0.1f, 1.0f, "%.2f");

}

void Gui::renderConfigWindow()
{
	if (!ImGui::Begin("Config", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	ImGui::SetWindowPos({ 10, 10 }, ImGuiCond_Once);

	if (ImGui::Button("Load config"))
	{
		CONFIG.Load();
	}
	if (ImGui::Button("Save config"))
	{
		CONFIG.Save();
	}

	ImGui::End();
}

void Gui::renderTestWindow()
{
	if (!ImGui::Begin("Test", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}
	ImGui::SetWindowPos({ 10,10 }, ImGuiCond_Once);

	ImGui::End();
}