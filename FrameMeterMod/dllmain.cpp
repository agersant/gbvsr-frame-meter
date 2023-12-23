// Included here as workaround for conflict between UE4SS macro named S and polyhook source
#include <polyhook2/Detour/x64Detour.hpp>

#include <Mod/CppUserModBase.hpp>

#include "meter.h"
#include "ui.h"

class FrameMeterMod : public RC::CppUserModBase
{
public:
	FrameMeterMod() : RC::CppUserModBase()
	{
		ModName = STR("FrameMeterMod");
		ModVersion = STR("1.0");
		ModDescription = STR("Adds a frame meter tool in training mode.");
		ModAuthors = STR("agersant");
	}

	void on_unreal_init() override
	{
		Meter::get_instance()->init();
		UI::get_instance()->init();
	}

	~FrameMeterMod() override
	{
	}
};

extern "C"
{
	__declspec(dllexport) RC::CppUserModBase *start_mod()
	{
		return new FrameMeterMod();
	}

	__declspec(dllexport) void uninstall_mod(RC::CppUserModBase *mod)
	{
		delete mod;
	}
}
