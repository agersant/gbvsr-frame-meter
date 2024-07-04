#SingleInstance Force
SetWorkingDir A_WorkingDir "\..\.."

#include "capture.ahk"
#include "dummy.ahk"
#include "tests.ahk"

SetKeyDelay 50, 50

WinActivate "Granblue Fantasy Versus: Rising"
sleep 200
if !WinActive("Granblue Fantasy Versus: Rising") {
    Exit
}

clean_artifacts()
reset_dummy_settings()
charlotta_6u_guard()