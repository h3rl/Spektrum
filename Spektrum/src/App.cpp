#include "App.h"

#include <Windows.h>

App::App() :
	m_window(new sf::RenderWindow()),
	m_audiosink(new AudioSink()),
	m_bInitialized(false)
{
	_D("App constructor called");
	Config::_Window& w = CONFIG.window;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	m_window->create(sf::VideoMode(w.width, w.height), "App", sf::Style::Close | sf::Style::Titlebar, settings);
	if (w.max_fps > 0)
	{
		m_window->setFramerateLimit(w.max_fps);
	}

	if (!m_gui.init(m_window))
	{
		return;
	}

	 // _MBYERROR("window initialized ???")
	//m_eventHandler.init(m_window);
	if (!m_audiosink->init())
	{
		return;
	}

	setAxis(Mathematical);
	setOrigin(BottomLeft);

	m_scene.init(m_window, m_audiosink);


	 //hide window if not debug
#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
	ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif

	m_bInitialized = true;
}

App::~App()
{
	_D("App destructor called");
}



void App::run()
{
	if (!m_bInitialized)
	{
		_D("App needs to be initialized before run is called");
		return;
	}

	sf::Clock clock;
	sf::Time elapsedTime;
	while (m_window->isOpen())
	{
		elapsedTime = clock.restart();
		processEvents();
		update(elapsedTime);
		render();
	}
	m_audiosink->stop();
	_D("Audiosink stopped");
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
	while (m_window->pollEvent(m_event))
	{
		ImGui::SFML::ProcessEvent(*m_window, m_event);

		switch (m_event.type)
		{
		case sf::Event::MouseWheelScrolled:

			scrolldelta = m_event.mouseWheelScroll.delta;
			if(!CONFIG.in_gui)
				zoomViewAt({ m_event.mouseWheelScroll.x, m_event.mouseWheelScroll.y }, *m_window, scrolldelta);
			break;

		case sf::Event::MouseMoved:
			//_DV(evt.mouseMove);
			pos = { m_event.mouseMove.x, m_event.mouseMove.y };
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
			if (m_event.key.code == sf::Keyboard::Escape)
			{
				m_window->close();
			}

			break;

		case sf::Event::Closed:
			m_window->close();
			break;
		}
	}

	if (!CONFIG.in_gui &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			sf::View view{ m_window->getView() };
			view.move(movedelta);
			m_window->setView(view);
		}
	}

	movedelta = { 0.0f,0.0f };
	scrolldelta = 0.0f;
}

void App::update(sf::Time dtTime)
{
	m_scene.update(dtTime);

	m_gui.update(dtTime);
}

void App::render()
{
	m_window->clear();

	m_scene.render();
	m_gui.render();

	m_window->display();
}

void App::setOrigin(WindowOrigin origin)
{
	sf::View view = m_window->getView();

	Config::_Window& w = CONFIG.window;

	float ww = (float)w.width;
	float wh = (float)w.height;

	switch (origin)
	{
	case Center:
		view.setCenter(0, 0);
		break;
	case TopLeft:
		view.setCenter(ww / 2, -wh / 2);
		break;
	case TopRight:
		view.setCenter(-ww / 2, -wh / 2);
		break;
	case BottomLeft:
		view.setCenter(ww / 2, wh/2);
		break;
	case BottomRight:
		view.setCenter(-ww / 2, wh / 2);
		break;
	default:
		break;
	}

	m_window->setView(view);
}

void App::setAxis(WindowAxis axis)
{
	sf::View view = m_window->getView();

	float width = (float)CONFIG.window.width;
	float height = (float)CONFIG.window.height;

	switch (axis)
	{
	case Mathematical:
		view.setSize(width, -height);
		break;
	case Computer:
		view.setSize(width, height);
		break;
	default:
		break;
	}
	m_window->setView(view);
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