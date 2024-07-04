#SingleInstance Force

#include "position_resets.ahk"
#include "character_selection.ahk"
#include "moves.ahk"

SetKeyDelay 20, 20
WinActivate "Granblue Fantasy Versus: Rising"

begin_capture()
{
	sendevent "{F8}"
}

charlotta_6u_guard()

charlotta_6u_guard() {
	select_character("charlotta")
	reset_to_right_corner()
	begin_capture()
	do_move("S+U")
}