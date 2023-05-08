#include "Scene.h"
#include "AudioSink.h"

Scene::Scene() :
	m_window(nullptr),
	m_audiosink(nullptr),
	m_pFloatData(nullptr),
	m_pByteData(nullptr)
{
}

void Scene::init(shared_ptr<Window> window, shared_ptr<AudioSink> audiosink)
{
	this->m_window = window;
	this->m_audiosink = audiosink;
	this->m_pFloatData = m_audiosink->fftOutput;
	this->m_pByteData = m_audiosink->byteFrequencyData;

	buildScene();
}


const int rectsCount = FFT_SIZE_HALF;

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

	// make sure its emtpy
	rects.clear();

	for (int i = 0; i < rectsCount; i++)
	{
		const float xpos = m_window->getSize().x * ((float)i / (rectsCount));
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
	const float height = m_window->getSize().y * 7.f / 8.f;
	const float width = m_window->getSize().x / rectsCount;

	for (int i = 0; i < FFT_SIZE/2; i++)
	{
		sf::RectangleShape& r = rects[i];

		const float barheight = m_audiosink->fftOutputDb[i]*height;
		//const float barheight = m_audiosink->byteFrequencyData[i];

		r.setSize(sf::Vector2f(width, barheight));
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