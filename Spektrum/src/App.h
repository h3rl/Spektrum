#pragma once
#include "stdafx.h"

#include "Scene.h"
#include "Config.h"
#include "Gui.h"
#include "EventHandler.h"
#include "AudioSink.h"


enum WindowOrigin : int
{
	Center,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

enum WindowAxis: int
{
	Mathematical, // x left, y up
	Computer // x right, y down
};

class App : private sf::NonCopyable
{
public:
							App();
							~App();

	void					run();

private:
	void					processEvents();
	void					update(sf::Time dtTime);
	void					render();

	void					setOrigin(WindowOrigin origin);
	void					setAxis(WindowAxis axis);

private:

	bool					m_bInitialized;

	shared_ptr<sf::RenderWindow>		m_window;
	Scene					m_scene;
	Gui						m_gui;
	shared_ptr<AudioSink>				m_audiosink;
	//EventHandler			m_eventHandler;
};