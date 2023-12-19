#include <DynamicOutput/DynamicOutput.hpp>
#include <Mod/CppUserModBase.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <stdio.h>

using namespace RC;
using namespace RC::Unreal;

const uint8_t HUD_VTABLE_INDEX_POST_RENDER = 0x6C0 / 8;
using HUDPostRender_sig = void (*)(uintptr_t);
PLH::VFuncMap hud_original_functions;

class FrameMeterMod : public CppUserModBase
{
public:
    std::unique_ptr<PLH::VFuncSwapHook> hud_hook;

    FrameMeterMod() : CppUserModBase()
    {
        ModName = STR("FrameMeterMod");
        ModVersion = STR("1.0");
        ModDescription = STR("Adds a frame meter tool in training mode.");
        ModAuthors = STR("agersant");
    }

    ~FrameMeterMod() override
    {
    }

    auto on_unreal_init() -> void override
    {
        install_hud_hook();
    }

    void install_hud_hook()
    {
        UClass *hud_class = UObjectGlobals::StaticFindObject<UClass *>(nullptr, nullptr, L"/Script/RED.REDHUD_Battle");
        if (!hud_class)
        {
            Output::send<LogLevel::Warning>(STR("HUD not found\n"));
            return;
        }

        UObject *default_hud = hud_class->GetClassDefaultObject();
        if (!default_hud)
        {
            Output::send<LogLevel::Warning>(STR("HUD DefaultObject not found\n"));
            return;
        }

        hud_hook = std::make_unique<PLH::VFuncSwapHook>(
            (uintptr_t)default_hud,
            PLH::VFuncMap({{HUD_VTABLE_INDEX_POST_RENDER, (uintptr_t)&FrameMeterMod::hud_post_render}}),
            &hud_original_functions);

        if (!hud_hook->hook())
        {
            Output::send<LogLevel::Warning>(STR("Failed to install HUD hook\n"));
            return;
        }
        Output::send<LogLevel::Normal>(STR("HUD hook installed\n"));
    }

    static void hud_post_render(uintptr_t hud)
    {
        Output::send<LogLevel::Normal>(STR("hud_post_render\n"));
        if (hud_original_functions.contains(HUD_VTABLE_INDEX_POST_RENDER))
        {
            ((HUDPostRender_sig)hud_original_functions.at(HUD_VTABLE_INDEX_POST_RENDER))(hud);
        }
    }
};

extern "C"
{
    __declspec(dllexport) RC::CppUserModBase *start_mod()
    {
        return new FrameMeterMod();
    }

    __declspec(dllexport) void uninstall_mod(CppUserModBase *mod)
    {
        delete mod;
    }
}
