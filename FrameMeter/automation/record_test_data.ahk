#SingleInstance Force
SetWorkingDir A_WorkingDir "\..\.."

#include "capture.ahk"
#include "character_selection.ahk"
#include "moves.ahk"
#include "position_resets.ahk"
#include "tests.ahk"

SetKeyDelay 50, 50

WinActivate "Granblue Fantasy Versus: Rising"
sleep 200
if !WinActive("Granblue Fantasy Versus: Rising") {
    Exit
}

clean_artifacts()
charlotta_6u_guard()