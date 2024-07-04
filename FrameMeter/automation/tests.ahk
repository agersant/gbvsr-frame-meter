charlotta_6u_guard() {
	select_character("charlotta")
	reset_to_right_corner()
	begin_capture()
	do_move("S+U")
	save_test_result("charlotta_6U_guard")
}