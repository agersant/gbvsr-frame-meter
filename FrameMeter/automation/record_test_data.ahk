#SingleInstance Force
SetWorkingDir A_WorkingDir "\..\.."

Esc:: ExitApp

#include "capture.ahk"
#include "dummy.ahk"
#include "tests.ahk"

SetKeyDelay 50, 50

WinActivate "Granblue Fantasy Versus: Rising"
Sleep 200
if !WinActive("Granblue Fantasy Versus: Rising") {
    Exit
}

clean_artifacts()
reset_dummy_settings()

cagliostro_S_guard_neutral_jump()
cagliostro_S_whiff()
charlotta_5U_whiff()
charlotta_fL_whiff()
charlotta_neutral_jump()
charlotta_neutral_jump_air_block()
charlotta_throw_hit()
charlotta_SU_guard()
charlotta_SU_whiff()
eustace_jU_whiff()
ferry_6S_guard()
katalina_2SU_guard()
katalina_2SU_ranged_guard()
katalina_S_guard()
katalina_S_ranged_guard()
nier_6SH_guard()
percival_5U_hit()
vane_2S_whiff_fL_whiff()
zeta_4S_whiff()
zeta_S_guard()