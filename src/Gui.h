#pragma once
#include "stdafx.h"

namespace Gui
{

extern bool hovered;
extern bool show_fps;
extern bool show_gui;

void init();
void preupdate();
void update(const sf::Time &dtTime);
void render();
void release();

} // namespace Gui