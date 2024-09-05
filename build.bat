@echo off
rem "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/sdk/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -S D:/gitcode/Spektrum -B D:/gitcode/Spektrum/build -G "Visual Studio 17 2022" -A x64
cmake --build D:/gitcode/Spektrum/build --config Debug
