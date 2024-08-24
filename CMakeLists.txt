# CMakeList.txt : CMake project for Spektrum, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(VCPKG_ROOT "C:/sdk/vcpkg")
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
INCLUDE("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project (Spektrum)

find_package(Boost REQUIRED)
find_package(libconfig REQUIRED)
find_package(imgui REQUIRED)
find_package(FFTW3f REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

# Add source to this project's executable.
#add_executable (Spektrum "Spektrum.cpp" "Spektrum.h")
file(GLOB SRC_FILES
    "src/*.h"
    "src/*.cpp"
)
add_executable(Spektrum ${SRC_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Spektrum PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(Spektrum PRIVATE
	fmt::fmt
	FFTW3::fftw3f
	Boost::boost
	sfml-system
	sfml-graphics
	sfml-window
	ImGui-SFML::ImGui-SFML
)