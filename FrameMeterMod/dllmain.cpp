#include <stdio.h>
#include <Mod/CppUserModBase.hpp>

class FrameMeterMod : public RC::CppUserModBase
{
public:
    FrameMeterMod() : CppUserModBase()
    {
        ModName = STR("FrameMeterMod");
        ModVersion = STR("1.0");
        ModDescription = STR("Adds a frame meter tool in training mode.");
        ModAuthors = STR("agersant");
        printf("FrameMeterMod says hello\n");
    }

    ~FrameMeterMod() override
    {
    }

    auto on_update() -> void override
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
