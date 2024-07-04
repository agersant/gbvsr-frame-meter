#include "menu_usage.ahk"
#include "utils.ahk"

dummy_blocking_options := ["does_not_block", "block_all", "after_initial_hit", "initial_hit_only", "hold_guard", "random"]
dummy_blocking_current := ""

reset_dummy_settings() {
	global dummy_blocking_current

	browse_to_menu("opponent", "behavior")
	loop 7 {
		SendEvent "{I}"
		SendEvent "{S}"
	}
	close_menu()

	dummy_blocking_current := dummy_blocking_options[1]
}

set_dummy_blocking(value) {
	global dummy_blocking_current

	if value == dummy_blocking_current {
		return
	}

	from := get_array_index(dummy_blocking_options, dummy_blocking_current)
	if from == -1 {
		throw Error("Unknown block option: " dummy_blocking_current)
	}

	to := get_array_index(dummy_blocking_options, value)
	if to == -1 {
		throw Error("Unknown block option: " value)
	}

	while from < to {
		from++
		SendEvent "{D}"
	}

	while from > to {
		from--
		SendEvent "{A}"
	}

	dummy_blocking_current := value
}