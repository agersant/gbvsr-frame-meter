cmake_minimum_required(VERSION 3.18)

project(FrameMeter)

# Dumping support

set(Dumping_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/extern/miniz.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/core/dump.cpp"
)

# Frame Meter

set(FrameMeterCore_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/core/battle.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/core/meter.cpp"
)

# Mod DLL

set(FrameMeterMod_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/debug.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/dllmain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/draw.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/game.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mod/ui.cpp"
)

add_library(FrameMeterMod SHARED
	$<$<CONFIG:Game__Test__Win64>:${Dumping_Sources}>
	${FrameMeterCore_Sources}
	${FrameMeterMod_Sources}
)
target_include_directories(FrameMeterMod PRIVATE src extern)
target_link_libraries(FrameMeterMod PUBLIC UE4SS)
add_dependencies(FrameMeterMod proxy)

# Tests executable

set(FrameMeterTest_Sources
	"${CMAKE_CURRENT_SOURCE_DIR}/src/test/main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/test/meter_snapshot.cpp"
)

add_executable(FrameMeterTest
	${Dumping_Sources}
	${FrameMeterCore_Sources}
	${FrameMeterTest_Sources}
)
target_compile_features(FrameMeterTest PRIVATE cxx_std_20)
target_include_directories(FrameMeterTest PRIVATE src extern)
target_compile_definitions(FrameMeterTest PRIVATE FRAME_METER_AUTOMATED_TESTS)
