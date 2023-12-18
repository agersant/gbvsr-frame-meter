#include <DynamicOutput/DynamicOutput.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <stdio.h>

using namespace RC;
using namespace RC::Unreal;

class FrameMeterMod : public CppUserModBase
{
public:
    FrameMeterMod() : CppUserModBase()
    {
        ModName = STR("FrameMeterMod");
        ModVersion = STR("1.0");
        ModDescription = STR("Adds a frame meter tool in training mode.");
        ModAuthors = STR("agersant");

        Output::send<LogLevel::Normal>(STR("FrameMeterMod says hello\n"));
    }

    ~FrameMeterMod() override
    {
    }

    auto on_update() -> void override
    {
    }

    auto on_unreal_init() -> void override
    {
        UObject *Object = UObjectGlobals::StaticFindObject<UObject *>(nullptr, nullptr, STR("/Script/CoreUObject.Object"));
        Output::send<LogLevel::Verbose>(STR("Object Name: {}\n"), Object->GetFullName());
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
