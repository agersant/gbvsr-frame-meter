cmake_minimum_required(VERSION 3.18)

set(TARGET FrameMeterMod)
project(${TARGET})


set(${TARGET}_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/dllmain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/meter.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/ui.cpp"
)

add_library(${TARGET} SHARED ${${TARGET}_Sources})
target_include_directories(${TARGET} PRIVATE .)
target_link_libraries(${TARGET} PUBLIC UE4SS)
