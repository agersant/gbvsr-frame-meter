-- Source files

local dumping_sources = {
	"extern/miniz.c",
	"src/core/dump.cpp",
}

local core_sources = {
	"src/core/battle.cpp",
	"src/core/hitbox.cpp",
	"src/core/math.cpp",
	"src/core/meter.cpp",
}

local mod_sources = {
	"src/mod/debug.cpp",
	"src/mod/dllmain.cpp",
	"src/mod/draw.cpp",
	"src/mod/game.cpp",
	"src/mod/ui.cpp",
}

local test_sources = {
	"src/test/main.cpp",
	"src/test/snapshot.cpp",
}

-- Mod DLL

target("FrameMeterMod")
add_deps("proxy")
add_rules("ue4ss.mod")
add_includedirs("src", "extern")
add_files(unpack(core_sources))
if is_mode("Game__Test__Win64") then
	add_files(unpack(dumping_sources))
end
add_files(unpack(mod_sources))

-- Tests executable

rule("targetdir_rule")
after_load(function(target)
	target:set("targetdir", path.join(os.projectdir(), "Binaries", get_config("mode"), target:name()))
end)

target("FrameMeterTest")
add_rules("targetdir_rule")
set_languages("cxx20")
add_includedirs("src", "extern")
add_files(unpack(core_sources))
add_files(unpack(dumping_sources))
add_files(unpack(test_sources))
add_defines("FRAME_METER_AUTOMATED_TESTS")
