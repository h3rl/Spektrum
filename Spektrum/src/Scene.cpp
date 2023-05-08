#include "Scene.h"
#include "AudioSink.h"

Scene::Scene()
{
	m_window = nullptr;
	m_audiosink = nullptr;
}

void Scene::init(shared_ptr<Window> window, shared_ptr<AudioSink> audiosink)
{
	this->m_window = window;
	this->m_audiosink = audiosink;
	buildScene();
}

void Scene::buildScene()
{
	// 0,0 rect
	//sf::RectangleShape r;
	//r.setFillColor(sf::Color::Blue);
	//r.setSize(sf::Vector2f(10, 10));
	//r.setPosition(sf::Vector2f(-5, -5));
	//rects.push_back(r);

	//// 100, 100 rect
	//sf::RectangleShape r2;
	//r2.setFillColor(sf::Color::Red);
	//r2.setSize(sf::Vector2f(25, 25));
	//r2.setPosition(sf::Vector2f(-12.5f + 50.f, -12.5f + 50.f));
	//rects.push_back(r2);

	const float wheight = m_window->getSize().y / 2.f;
	const int arr_size = sizeof(m_audiosink->fftOutput) / sizeof(m_audiosink->fftOutput[0]);

	// make sure its emtpy
	rects.clear();

	for (int i = 0; i < arr_size; i++)
	{
		const float xpos = m_window->getSize().x * ((float)i / (float)arr_size);
		const float ypos = 0;// m_window->getSize().y * 1 / 8;

		sf::RectangleShape r;
		r.setFillColor(sf::Color::Blue);
		r.setPosition(sf::Vector2f(xpos, ypos));
		rects.push_back(r);
	}
}

//TODO:finish
void Scene::update(sf::Time dtTime)
{
	float wheight = m_window->getSize().y * 7.f / 8.f;
	
	const int arr_size = sizeof(m_audiosink->fftOutput) / sizeof(m_audiosink->fftOutput[0]);

	for (int i = 0; i < arr_size; i++)
	{
		sf::RectangleShape& r = rects[i];
		r.setSize(sf::Vector2f(1, abs(m_audiosink->fftOutput[i] * wheight)));
	}
}

//TODO:finish
void Scene::render()
{
	for (const auto& r : rects)
	{
		m_window->draw(r);
	}
}