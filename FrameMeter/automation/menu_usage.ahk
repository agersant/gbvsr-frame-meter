menu_tabs := ["main", "quick_menu", "battle", "opponent", "display"]

menu_entries := Map()
menu_entries["quick_menu"] := ["position_reset", "character_1p", "character_2p", "stage", "music", "character_specific_settings"]

current_menu_tab_index := 1
current_menu_entry_index := 1

tab_to_index(tab_name) {
	for index, name in menu_tabs {
		if name == tab_name {
			return index
		}
	}
}

tab_entry_to_index(tab_name, entry_name) {
	for index, entry in menu_entries[tab_name] {
		if entry == entry_name {
			return index
		}
	}
}

input_previous_tab() {
	global current_menu_tab_index
	global current_menu_entry_index

	sendevent "{O}"
	current_menu_tab_index--
	current_menu_entry_index := 1
}

input_next_tab() {
	global current_menu_tab_index
	global current_menu_entry_index

	sendevent "{P}"
	current_menu_tab_index++
	current_menu_entry_index := 1
}

input_previous_entry() {
	global current_menu_entry_index

	sendevent "{W}"
	current_menu_entry_index--
}

input_next_entry() {
	global current_menu_entry_index

	sendevent "{S}"
	current_menu_entry_index++
}

browse_to_menu(tab_name, entry) {
	sendevent "{Tab}"

	sleep 200

	to_tab := tab_to_index(tab_name)
	if to_tab != current_menu_tab_index {
		while current_menu_tab_index < to_tab {
			input_next_tab()
		}

		while current_menu_tab_index > to_tab {
			input_previous_tab()
		}
	}

	to_option := tab_entry_to_index(tab_name, entry)
	if to_option != current_menu_entry_index {
		while current_menu_entry_index < to_option {
			input_next_entry()
		}

		while current_menu_entry_index > to_option {
			input_previous_entry()
		}
	}
}

close_menu() {
	sendevent "{Tab}"
}