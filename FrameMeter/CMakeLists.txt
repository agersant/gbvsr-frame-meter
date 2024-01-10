cmake_minimum_required(VERSION 3.18)

project(FrameMeter)

set(FrameMeterCore_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/core/battle.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/core/meter.cpp"
)

# Mod DLL

set(FrameMeterMod_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/debug.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/dllmain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/ui.cpp"
)

add_library(FrameMeterMod SHARED ${FrameMeterCore_Sources} ${FrameMeterMod_Sources})
target_include_directories(FrameMeterMod PRIVATE src)
target_link_libraries(FrameMeterMod PUBLIC UE4SS)

# Tests executable

set(FrameMeterTest_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/test/main.cpp"
)

add_executable(FrameMeterTest ${FrameMeterCore_Sources} ${FrameMeterTest_Sources})
target_compile_features(FrameMeterTest PRIVATE cxx_std_20)
target_include_directories(FrameMeterTest PRIVATE src)
