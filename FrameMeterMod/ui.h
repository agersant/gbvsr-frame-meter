#include <polyhook2/Virtuals/VFuncSwapHook.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/UFunction.hpp>

class UI
{
public:
	static UI *get_instance();
	void init();

	UI(UI &other) = delete;
	void operator=(const UI &) = delete;

protected:
	static UI *instance;
	UI();

	std::unique_ptr<PLH::VFuncSwapHook> hud_hook = nullptr;
	PLH::VFuncMap hud_original_functions = {};
	RC::Unreal::UFunction *draw_rect = nullptr;

	static void draw_callback(uintptr_t hud);
	void draw(RC::Unreal::AActor *hud);
};
