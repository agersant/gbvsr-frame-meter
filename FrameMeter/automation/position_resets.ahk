reset_to_midscreen() {
	sendevent "{S down}{1}{S up}"
	sleep 1000
}

reset_to_left_corner() {
	sendevent "{A down}{1}{A up}"
	sleep 1000
}

reset_to_right_corner() {
	sendevent "{D down}{1}{D up}"
	sleep 1000
}