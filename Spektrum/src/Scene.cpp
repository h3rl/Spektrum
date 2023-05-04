#include "Scene.h"
#include "AudioSink.h"

Scene::Scene() :
window(nullptr),
config(nullptr)
{
}

void Scene::init(sf::RenderWindow& window, AudioSink& _soundhandler)
{
	this->window = &window;
	this->audiosink = &_soundhandler;
	buildScene();
	_D("Scene generated");
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

	float wheight = window->getSize().y / 2.f;

	const int arr_size = sizeof(audiosink->fftOutput) / sizeof(audiosink->fftOutput[0]);

	for (int i = 0; i < arr_size; i++)
	{
		float xpos = window->getSize().x * i / (float)arr_size;
		float ypos = window->getSize().y * 1/8;

		sf::RectangleShape r;
		r.setFillColor(sf::Color::Blue);
		r.setPosition(sf::Vector2f(xpos, ypos));
		rects.push_back(r);
	}
}


void Scene::init(sf::RenderWindow& window, AudioSink& _soundhandler, sf::RenderStates& state)
{
	this->Scene::init(window, _soundhandler);
	renderStates = &state;
}

//TODO:finish
void Scene::update(sf::Time dtTime)
{
	float wheight = window->getSize().y * 7 / 8;
	float wwidth = window->getSize().x;
	
	const int arr_size = sizeof(audiosink->fftOutput) / sizeof(audiosink->fftOutput[0]);

	for (int i = 0; i < arr_size; i++)
	{
		sf::RectangleShape& r = rects[i];
		r.setSize(sf::Vector2f(1, abs(audiosink->fftOutput[i] * wheight)));
	}
}

//TODO:finish
void Scene::render(sf::RenderStates rs)
{
	for (const auto& r : rects)
	{
		window->draw(r);
	}
}