#include "capture.ahk"
#include "character_selection.ahk"
#include "config.ahk"
#include "dummy.ahk"
#include "moves.ahk"
#include "position_resets.ahk"


_2b_backdash() {
	select_character("2b")
	reset_to_midscreen()
	begin_capture()

	Send "{" key_move_left " down}"
	sleep_frames(2)
	Send "{" key_move_left " up}"

	sleep_frames(2)

	Send "{" key_move_left " down}"
	sleep_frames(2)
	Send "{" key_move_left " up}"

	save_test_result("2b_backdash")
}

_2b_evade() {
	select_character("2b")
	reset_to_midscreen()
	begin_capture()
	do_move("evade")
	save_test_result("2b_evade")
}

avatar_belial_2S_U_ranged_whiff() {
	select_character("avatar_belial")
	reset_to_midscreen()
	begin_capture()
	do_move("2S")
	loop 10 {
		sleep_frames(1)
		Send "{" key_unique " down}"
		sleep_frames(2)
		Send "{" key_unique " up}"
	}
	save_test_result("avatar_belial_2S_U_ranged_whiff")
}

cagliostro_S_guard_neutral_jump() {
	select_character("cagliostro")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("S")
	Send "{" key_jump " down}"
	sleep_frames(50)
	Send "{" key_jump " up}"
	save_test_result("cagliostro_S_guard_neutral_jump")
}

cagliostro_S_whiff() {
	select_character("cagliostro")
	reset_to_midscreen()
	begin_capture()
	do_move("S")
	save_test_result("cagliostro_S_whiff")
}

charlotta_5U_whiff() {
	select_character("charlotta")
	reset_to_midscreen()
	begin_capture()
	do_move("U")
	save_test_result("charlotta_5U_whiff")
}

charlotta_autocombo_overhead_guard() {
	select_character("charlotta")
	reset_to_right_corner()
	begin_capture()
	do_move("M")
	do_move("M")
	loop 20 {
		sleep_frames(1)
		do_move("6+H")
	}
	save_test_result("charlotta_autocombo_overhead_guard")
}

charlotta_fL_whiff() {
	select_character("charlotta")
	reset_to_midscreen()
	begin_capture()
	do_move("fL")
	save_test_result("charlotta_fL_whiff")
}

charlotta_neutral_jump() {
	select_character("charlotta")
	reset_to_midscreen()
	begin_capture()
	SendEvent "{" key_jump "}"
	save_test_result("charlotta_neutral_jump")
}

charlotta_neutral_jump_air_block() {
	select_character("charlotta")
	reset_to_midscreen()
	begin_capture()
	SendEvent "{" key_jump "}"
	Send "{" key_block " down}"
	save_test_result("charlotta_neutral_jump_air_block")
	Send "{" key_block " up}"
}

charlotta_throw_hit() {
	select_character("charlotta")
	reset_to_right_corner()
	begin_capture()
	do_move("Throw")
	save_test_result("charlotta_throw_hit")
}

charlotta_SU_guard() {
	select_character("charlotta")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("S+U")
	save_test_result("charlotta_S+U_guard")
}

charlotta_SU_whiff() {
	select_character("charlotta")
	reset_to_midscreen()
	begin_capture()
	do_move("S+U")
	save_test_result("charlotta_S+U_whiff")
}

eustace_jU_whiff() {
	select_character("eustace")
	reset_to_midscreen()
	begin_capture()
	Send "{" key_jump " down}"
	sleep_frames(1)
	do_move("U")
	Send "{" key_jump " up}"
	save_test_result("eustace_jU_whiff")
}

ferry_6S_guard() {
	select_character("ferry")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("6S")
	save_test_result("ferry_6S_guard")
}

katalina_2SU_guard() {
	select_character("katalina")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("2S+U")
	save_test_result("katalina_2S+U_guard")
}

katalina_2SU_ranged_guard() {
	select_character("katalina")
	set_dummy_blocking("block_all")
	reset_to_midscreen()
	begin_capture()
	do_move("2S+U")
	save_test_result("katalina_2S+U_ranged_guard")
}

katalina_S_guard() {
	select_character("katalina")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("S")
	save_test_result("katalina_S_guard")
}

katalina_S_ranged_guard() {
	select_character("katalina")
	set_dummy_blocking("block_all")
	reset_to_midscreen()
	begin_capture()
	do_move("S")
	save_test_result("katalina_S_ranged_guard")
}

nier_6SH_guard() {
	select_character("nier")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("6S+H")
	save_test_result("nier_6SH_guard")
}

percival_5U_hit() {
	select_character("percival")
	reset_to_right_corner()
	begin_capture()
	do_move("U")
	save_test_result("percival_5U_hit")
}

vane_2S_whiff_fL_whiff() {
	select_character("vane")
	reset_to_midscreen()
	begin_capture()
	do_move("2S")
	sleep_frames(35)
	loop 3 {
		do_move("fL")
		sleep_frames(5)
	}
	save_test_result("vane_2S_whiff_fL_whiff")
}

zeta_4S_whiff() {
	select_character("zeta")
	reset_to_midscreen()
	begin_capture()
	do_move("4S")
	save_test_result("zeta_4S_whiff")
}

zeta_S_guard() {
	select_character("zeta")
	set_dummy_blocking("block_all")
	reset_to_right_corner()
	begin_capture()
	do_move("S")
	save_test_result("zeta_S_guard")
}

zeta_S_ranged_guard() {
	select_character("zeta")
	set_dummy_blocking("block_all")
	reset_to_midscreen()
	begin_capture()
	do_move("S")
	save_test_result("zeta_S_ranged_guard")
}
