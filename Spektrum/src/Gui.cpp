#include "stdafx.h"

#include "Gui.h"

Gui::Gui()
{
}

Gui::~Gui()
{
	ImGui::SFML::Shutdown();
}

bool Gui::init(shared_ptr<sf::RenderWindow> window)
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

	return true;
}

void Gui::update(sf::Time dtTime)
{
	ImGui::SFML::Update(*m_window,dtTime);

	CONFIG.in_gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

	m_fpsText.setString(std::to_string((int)( 1 / dtTime.asSeconds())));
}

void Gui::render()
{
	//renderConfigWindow();
	//renderTestWindow();
	ImGui::SFML::Render(*m_window);

	m_fpsText.setPosition({ 0,0 });

	sf::View oldview = m_window->getView();

	// Reset view to default, so we can draw the text
	m_window->setView(m_window->getDefaultView());
	m_window->draw(m_fpsText);
	
	// Set it back to the original view
	m_window->setView(oldview);
}

void Gui::renderConfigWindow()
{
	if (!ImGui::Begin("Config", 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Load config"))
	{
		CONFIG.config.load = true;
	}
	if (ImGui::Button("Save config"))
	{
		CONFIG.config.save = true;
	}
	float x = m_window->getSize().x - 200;
	ImGui::SetWindowPos({ x, 10 }, ImGuiCond_Once);
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