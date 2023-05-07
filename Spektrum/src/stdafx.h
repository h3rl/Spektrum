#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include "imgui.h"
#include "imgui-SFML.h"

#include <Thor/Vectors.hpp>
#include <Thor/Math.hpp>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <fftw3.h>

#include "Constants.h"

using std::string;
using std::shared_ptr;
using std::make_shared;

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

#define _D(x) std::cout << x << std::endl;
#define _DV(vec) _D(vec.x << " " << vec.y);
#define clamp(v,mn,mx) ((v < mn) ? mn : (v > mx) ? mx : v);
#define _TODO(x)
#define _MBYERROR(x)