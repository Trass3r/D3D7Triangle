cmake_minimum_required(VERSION 3.20)

# need to fetch toolchain before first project() call
include(FetchContent)
FetchContent_Declare(
    llvm_mingw
    URL https://github.com/mstorsjo/llvm-mingw/releases/download/20260127/llvm-mingw-20260127-ucrt-ubuntu-22.04-x86_64.tar.xz
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/llvm-mingw
)
FetchContent_MakeAvailable(llvm_mingw)

set(LLVM_MINGW_PATH "${llvm_mingw_SOURCE_DIR}")

project(D3DTriangle)

add_compile_options($<$<CONFIG:Debug>:-Og> -g -fno-exceptions -ffunction-sections -fdata-sections)
add_compile_options(-Wall -Wextra -Wno-missing-field-initializers -Wshadow -Wconversion)
add_link_options(-static -Wl,--gc-sections,--icf=safe) #,--strip-all)

add_executable(D3D7Triangle WIN32 d3d7triangle.cpp)
target_link_libraries(D3D7Triangle ddraw dxguid)

add_executable(D3D11Cube WIN32 d3d11cube.cpp)
target_link_libraries(D3D11Cube d3d11 d3dcompiler user32)
