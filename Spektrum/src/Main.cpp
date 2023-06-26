#include "stdafx.h"

#include "Config.h"
#include "Window.h"
#include "Gui.h"
#include "AudioSink.h"
#include "Scene.h"

#include <Windows.h>

// Forward declerations
void render();
void update(const sf::Time& dt);
void processEvents();

void pbCallback(boost::circular_buffer<float>& dest, float* pData, const UINT& size);

int main()
{
    ShowWindow(GetConsoleWindow(), SW_SHOW);

    // Initialization
    if (config.LoadOnStartup())
    {
        config.Load();
    }

    if (!g_window.init())
    {
        return EXIT_FAILURE;
    }
    if (!g_gui.init())
    {
        return EXIT_FAILURE;
    }

    if (!g_audiosink.init())
    {
        return EXIT_FAILURE;
    }

	g_audiosink.setPbCallback(pbCallback);

    if (!g_scene.init())
    {
        return EXIT_FAILURE;
    }
	// Main loop
	sf::Clock clock;
	sf::Time elapsedTime;
    while (g_window.isOpen())
    {
        elapsedTime = clock.restart();
        processEvents();
        update(elapsedTime);
        render();
    }

	return EXIT_SUCCESS;
}
/*
sf::Vector2i oldpos, pos;
sf::Vector2f movedelta;
float scrolldelta;
bool firstTimeDelta = true;

void zoomViewAt(sf::Vector2i pixel, Window& window, float wheeldelta)
{
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		return;

	float zoom = 1.1f;
	if (wheeldelta > 0)
	{
		zoom = 1.0f / zoom;
	}

	const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
	sf::View view{ window.getView() };
	view.zoom(zoom);
	window.setView(view);
	const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
	const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
	view.move(offsetCoords);
	window.setView(view);
}
*/

void pbCallback(boost::circular_buffer<float>& dest, float* pData, const UINT& size)
{
	for (UINT i = 0; i < size; i += 4)
	{
		const float downsampled = 0.25f * (pData[i] + pData[i + 1] + pData[i + 2] + pData[i + 3]); // average to create mono
		dest.push_front(downsampled);
	}
}

void processEvents()
{
	sf::Event evt;
	while (g_window.pollEvent(evt))
	{
		ImGui::SFML::ProcessEvent(g_window, evt);

		switch (evt.type)
		{
		//case sf::Event::MouseWheelScrolled:
		//{
		//	scrolldelta = evt.mouseWheelScroll.delta;
		//	if (!state.in_gui)
		//		zoomViewAt({ evt.mouseWheelScroll.x, evt.mouseWheelScroll.y }, g_window, scrolldelta);
		//	break;
		//}
		//case sf::Event::MouseMoved:
		//{
		//	//_DV(evt.mouseMove);
		//	pos = { evt.mouseMove.x, evt.mouseMove.y };
		//	if (firstTimeDelta)
		//	{
		//		firstTimeDelta = false;
		//		movedelta = { 0.0f,0.0f };
		//	}
		//	else
		//	{
		//		movedelta = sf::Vector2f(oldpos - pos);
		//	}
		//	oldpos = pos;

		//	break;
		//}

		//case sf::Event::MouseEntered:
		//	break;
		//case sf::Event::MouseLeft:
		//	break;
		//case sf::Event::Resized:
		//	break;
		//case sf::Event::LostFocus:
		//	break;
		//case sf::Event::GainedFocus:
		//	break;
		//case sf::Event::TextEntered:
		//	//_D((char)evt.text.unicode);
		//	break;

		case sf::Event::KeyPressed:
		{
			if (evt.key.code == sf::Keyboard::Escape)
			{
				g_window.close();
			}

			break;
		}
		case sf::Event::Resized:
		{
			// update view
			g_window.updateSize();

			// regenerate scene
			g_scene.buildScene();
			break;
		}
		case sf::Event::Closed:
		{
			g_window.close();
			break;
		}
		}
	}
}



void update(const sf::Time& dtTime)
{
	g_gui.preupdate();

	g_audiosink.update(dtTime);

	g_scene.update(dtTime);

	g_gui.update(dtTime);
}

void render()
{
	g_window.clear();

	g_scene.render();
	g_gui.render();

	g_window.display();
}
//void App::handleInputMouseMove(sf::Event::MouseMoveEvent evt)
//{
//	if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
//	{
//		sf::Vector2f delta{ (float)evt.x, (float)evt.y };
//		delta *= 0.01f;
//		sf::View view = window.getView();
//		view.move(delta);
//		window.setView(view);
//	}
//}