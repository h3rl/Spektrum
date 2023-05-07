#include "EventHandler.h"

EventHandler::EventHandler() :
	firstTimeDelta(false)
{
	mouse.pos = { 0.0f,0.0f };
	mouse.delta = { 0.0f,0.0f };

	for (size_t i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		mo_pressed.insert({(sf::Mouse::Button)i, false});
	}

	for (size_t i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		kb_pressed.insert({ (sf::Keyboard::Key)i, false });
	}
}

void EventHandler::handleEvents()
{
	sf::Vector2f oldpos, newpos;

	this->reset();

	sf::Event evt;

	while (m_window->pollEvent(evt))
	{
		ImGui::SFML::ProcessEvent(*m_window, evt);

		switch (evt.type)
		{
		case sf::Event::MouseButtonPressed:
			setpressed(evt.mouseButton.button, true);
			break;

		case sf::Event::MouseButtonReleased:
			setpressed(evt.mouseButton.button, false);
			break;


		case sf::Event::KeyPressed:
			setpressed(evt.key.code, true);
			break;

		case sf::Event::KeyReleased:
			setpressed(evt.key.code, false);
			break;

		case sf::Event::MouseWheelScrolled:
			mouse.wheel.delta = evt.mouseWheelScroll.delta;
			//mouse.pos.x = evt.mouseWheelScroll.x;
			//mouse.pos.y = evt.mouseWheelScroll.y;
			break;

		case sf::Event::MouseMoved:
			newpos = { (float)evt.mouseMove.x, (float)evt.mouseMove.y };

			if (firstTimeDelta)
			{
				firstTimeDelta = false;
				mouse.delta = { 0.0f,0.0f };
			}
			else
			{
				oldpos = mouse.pos;
				mouse.delta = oldpos - newpos;
			}
			mouse.pos = newpos;

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

		case sf::Event::Closed:
			m_window->close();
			break;
		}
	}
}

void EventHandler::init(shared_ptr<sf::RenderWindow> window)
{
	this->m_window = window;
}

bool EventHandler::pressed(sf::Keyboard::Key key)
{
	return kb_pressed.at(key);
}

bool EventHandler::pressed(sf::Mouse::Button button)
{
	return mo_pressed.at(button);
}

void EventHandler::setpressed(sf::Keyboard::Key key, bool value)
{
	kb_pressed.at(key) = value;
}

void EventHandler::setpressed(sf::Mouse::Button button, bool value)
{
	mo_pressed.at(button) = value;
}

void EventHandler::reset()
{
	mouse.delta = { 0.0f,0.0f };
	mouse.wheel.delta = 0.0f;
}
