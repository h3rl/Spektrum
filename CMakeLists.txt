cmake_minimum_required (VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (Spektrum)

# Add source to this project's executable.
add_executable(Spektrum 
	"src/main.cpp"
	"src/dft.cpp"
	"src/dft.h"
	# "src/audio_post_processing.cpp"
	# "src/audio_post_processing.h"
	"src/audio_windowing.cpp"
	"src/audio_windowing.h"
	"src/audio_sink.cpp"
	"src/audio_sink.h"
	"src/config.cpp"
	"src/config.h"
	"src/color.cpp"
	"src/color.h"
	"src/gui.cpp"
	"src/gui.h"
	"src/logging.h"
	"src/scene_histogram.cpp"
	"src/scene_histogram.h"
	# "src/scene.cpp"
	# "src/scene.h"
	"src/stdafx.cpp"
	"src/stdafx.h"
	"src/window.cpp"
	"src/window.h"
)

find_package(Boost REQUIRED)
target_link_libraries(Spektrum PRIVATE Boost::boost)

find_package(libconfig CONFIG REQUIRED)
target_link_libraries(Spektrum PRIVATE libconfig::libconfig libconfig::libconfig++)

find_package(imgui REQUIRED)
target_link_libraries(Spektrum PRIVATE imgui::imgui)

find_package(FFTW3 REQUIRED)
target_link_libraries(Spektrum PRIVATE FFTW3::fftw3)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(Spektrum PRIVATE fmt::fmt)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
target_link_libraries(Spektrum PRIVATE sfml-system sfml-network sfml-graphics sfml-window)

find_package(ImGui-SFML CONFIG REQUIRED)
target_link_libraries(Spektrum PRIVATE ImGui-SFML::ImGui-SFML)

target_precompile_headers(Spektrum PRIVATE "src/stdafx.h")