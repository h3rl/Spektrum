#include "App.h"

#include <Windows.h>

App::App()
{
	Config::_Window& w = CONFIG.window;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	window.create(sf::VideoMode(w.width, w.height), "App", sf::Style::Close | sf::Style::Titlebar, settings);
	if (w.max_fps > 0)
	{
		window.setFramerateLimit(w.max_fps);
	}

	gui.init(window), // _MBYERROR("window initialized ???")
	evtHandler.init(window);
	audiosink.init();
	audiosink.start();

	setAxis(Mathematical);
	setOrigin(BottomLeft);

	Scene.init(window, audiosink);

	_D("App initialized");

	 //hide window if not debug
#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif

}

App::~App()
{
	audiosink.stop();
}



void App::run()
{
	sf::Clock clock;
	sf::Time elapsedTime = sf::Time::Zero;
	while (window.isOpen())
	{
		elapsedTime = clock.restart();
		processEvents();
		update(elapsedTime);
		render();
	}
}

sf::Vector2i oldpos, pos;
sf::Vector2f movedelta;
float scrolldelta;
bool firstTimeDelta = true;

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float wheeldelta)
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

void App::processEvents()
{
	while (window.pollEvent(evt))
	{
		ImGui::SFML::ProcessEvent(window, evt);

		switch (evt.type)
		{
		case sf::Event::MouseWheelScrolled:

			scrolldelta = evt.mouseWheelScroll.delta;
			if(!CONFIG.in_gui)
				zoomViewAt({ evt.mouseWheelScroll.x, evt.mouseWheelScroll.y }, window, scrolldelta);
			break;

		case sf::Event::MouseMoved:
			//_DV(evt.mouseMove);
			pos = { evt.mouseMove.x, evt.mouseMove.y };
			if (firstTimeDelta)
			{
				firstTimeDelta = false;
				movedelta = { 0.0f,0.0f };
			}
			else
			{
				movedelta = sf::Vector2f(oldpos - pos);
			}
			oldpos = pos;

			break;

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

		case sf::Event::MouseButtonPressed:
			if (evt.key.code == sf::Keyboard::Escape)
			{
				window.close();
			}

			break;

		case sf::Event::Closed:
			window.close();
			break;
		}
	}

	if (!CONFIG.in_gui &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::View view{ window.getView() };
			view.move(movedelta);
			window.setView(view);
		}
	}

	movedelta = { 0.0f,0.0f };
	scrolldelta = 0.0f;
}

void App::update(sf::Time dtTime)
{
	Scene.update(dtTime);

	gui.update(dtTime);
}

void App::render()
{
	window.clear();

	sf::RenderStates rs = renderStates;
	//sf::Vector2f mpos = rs.transform.getInverse().transformPoint(evtHandler.mouse.pos);
	//rs.transform.translate(mpos);
	//rs.transform.scale(CONFIG.window.zoom, CONFIG.window.zoom);

	Scene.render(rs);
	gui.render(rs);

	window.display();
}

void App::setOrigin(WindowOrigin origin)
{
	sf::View view = window.getView();
	_D(view.getCenter().x << " " << view.getCenter().y);

	Config::_Window& w = CONFIG.window;

	switch (origin)
	{
	case Center:
		view.setCenter(0, 0);
		break;
	case TopLeft:
		view.setCenter(w.width / 2, -w.height / 2);
		break;
	case TopRight:
		view.setCenter(-w.width / 2, -w.height / 2);
		break;
	case BottomLeft:
		view.setCenter(w.width / 2, w.height/2);
		break;
	case BottomRight:
		view.setCenter(-w.width / 2, w.height / 2);
		break;
	default:
		break;
	}

	window.setView(view);
}

void App::setAxis(WindowAxis axis)
{
	sf::View view = window.getView();
	switch (axis)
	{
	case Mathematical:
		view.setSize(CONFIG.window.width, -CONFIG.window.height);
		break;
	case Computer:
		view.setSize(CONFIG.window.width, CONFIG.window.height);
		break;
	default:
		break;
	}
	window.setView(view);
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