#pragma once
#include "stdafx.h"

#include "Window.h"
#include "Scene.h"
#include "Config.h"
#include "Gui.h"
#include "EventHandler.h"
#include "AudioSink.h"

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
private:

	bool					m_bInitialized;

	shared_ptr<Window>		m_window;
	Scene					m_scene;
	Gui						m_gui;
	shared_ptr<AudioSink>	m_audiosink;
	//EventHandler			m_eventHandler;
};