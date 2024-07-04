#include "capture.ahk"
#include "character_selection.ahk"
#include "dummy.ahk"
#include "moves.ahk"
#include "position_resets.ahk"

charlotta_6u_guard() {
	select_character("charlotta")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("S+U")
	save_test_result("charlotta_6U_guard")
}