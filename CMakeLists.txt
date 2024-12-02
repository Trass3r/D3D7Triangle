cmake_minimum_required(VERSION 3.20)

# need to fetch toolchain before first project() call
include(FetchContent)
FetchContent_Declare(
    llvm_mingw
    URL https://github.com/mstorsjo/llvm-mingw/releases/download/20250319/llvm-mingw-20250319-ucrt-ubuntu-20.04-x86_64.tar.xz
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/llvm-mingw
)
FetchContent_MakeAvailable(llvm_mingw)

set(LLVM_MINGW_PATH "${llvm_mingw_SOURCE_DIR}")

project(D3D11Triangle)

add_compile_options($<$<CONFIG:Debug>:-Og> -g -fno-exceptions -ffunction-sections -fdata-sections)
add_compile_options(-Werror=all -Wextra -Wno-missing-field-initializers -Wshadow -Wconversion)
add_link_options(-Wl,--gc-sections,--icf=safe,--strip-all)
add_executable(D3D11Triangle main.cpp)
target_link_libraries(D3D11Triangle ddraw dxguid)
