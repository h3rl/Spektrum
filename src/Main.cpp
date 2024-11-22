
#include "logging.h"
#include "stdafx.h"
#include "main.h"

#include "dft.h"
#include "audio_sink.h"
#include "beat_detector.h"
#include "config.h"
#include "scene.h"
#include "window.h"
#include "gui.h"

#include <Windows.h>

// Forward declerations
void render();
void update(const sf::Time &dt);
void processEvents();

int main()
{
	ShowWindow(GetConsoleWindow(), SW_SHOW);

	// Initialization
	Config::Load();

	Window::init();

	Gui::init();

	AudioSink::init();

	//AudioSink::set_process_packet_buffer_callback((pfn_ProcessPacketBuffer)process_packet_buffer_callback);

	// Main loop
	sf::Clock clock;
	sf::Time elapsedTime;
	while (p_window->isOpen())
	{
		elapsedTime = clock.restart();
		processEvents();
		update(elapsedTime);
		render();
	}

	BeatDetector::release();
	AudioSink::release();

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

void process_packet_buffer_callback(float *pData, const unsigned int size)
{
	for (UINT i = 0; i < size; i += 4)
	{
		if (i >= 10)
			break;
		const float downsampled =
		    0.25f * (pData[i] + pData[i + 1] + pData[i + 2] + pData[i + 3]); // average to create mono
		log_msg("{:.4f} ", abs(downsampled));
	}
	log_msg_ln("");
}

void processEvents()
{
	sf::Event evt;
	while (p_window->pollEvent(evt))
	{
		ImGui::SFML::ProcessEvent(*p_window, evt);

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
				p_window->close();
			}

			break;
		}
		case sf::Event::Resized:
		{
			// update view
			// Window::updateSize();

			// regenerate scene
			// Scene::buildScene();
			Scene::build();
			break;
		}
		case sf::Event::Closed:
		{
			p_window->close();
			break;
		}

		default:
			break;
		}
	}
}

void update(const sf::Time &dtTime)
{
	Window::update();

	Gui::preupdate();

	AudioSink::update();
	BeatDetector::update();

	// Scene::update(dtTime);
	Scene::update();

	Gui::update(dtTime);
}

void render()
{
	p_window->clear();

	// Scene::render();
	Scene::render();
	Gui::render();

	p_window->display();
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