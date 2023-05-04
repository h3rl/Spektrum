#include "stdafx.h"

#include "Gui.h"

Gui::Gui() :
	window(nullptr)
{
	if (!font.loadFromFile("Assets\\arial.ttf")) return;

	_D("font loaded");

	fpsText.setFont(font);
	fpsText.setString("");
	fpsText.setCharacterSize(14); // in pixels, not points!
	fpsText.setFillColor(sf::Color(110, 255, 0));
	//fpsText.setStyle(sf::Text::Bold | sf::Text::Underlined);

}

Gui::~Gui()
{
	ImGui::SFML::Shutdown();
}

void Gui::init(sf::RenderWindow& window)
{
	ImGui::SFML::Init(window);
	this->window = &window;
	_D("Gui initialized");
}

void Gui::update(sf::Time dtTime)
{
	ImGui::SFML::Update(*window,dtTime);

	CONFIG.in_gui = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

	fpsText.setString(std::to_string((int)( 1 / dtTime.asSeconds())));
}

void Gui::render(sf::RenderStates& rs)
{
	//renderConfigWindow();
	//renderTestWindow();
	ImGui::SFML::Render(*window);

	fpsText.setPosition({ 0,0 });

	sf::View view{ window->getView() };

	// Reset view to default, so we can draw the text
	window->setView(window->getDefaultView());
	window->draw(fpsText);
	
	// Set it back to the original view
	window->setView(view);
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
	float x = window->getSize().x - 200;
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