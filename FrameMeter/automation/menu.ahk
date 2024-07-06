menu_tabs := ["main", "quick_menu", "battle", "opponent", "display"]

menu_entries := Map()
menu_entries["quick_menu"] := ["position_reset", "character_1p", "character_2p", "stage", "music", "character_specific_settings"]
menu_entries["opponent"] := ["behavior", "training_dummy_stance", "blocking", "block_switching", "throw_break", "recovery", "counter_hit", "counter_attack_settings", "dummy_action_recording_settings"]

current_menu_tab_index := 1
current_menu_entry_index := 1

input_previous_tab() {
    global current_menu_tab_index
    global current_menu_entry_index

    SendEvent "{O}"
    Sleep 100

    current_menu_tab_index--
    current_menu_entry_index := 1
}

input_next_tab() {
    global current_menu_tab_index
    global current_menu_entry_index

    SendEvent "{P}"
    Sleep 100

    current_menu_tab_index++
    current_menu_entry_index := 1
}

input_previous_entry() {
    global current_menu_entry_index

    SendEvent "{W}"
    current_menu_entry_index--
}

input_next_entry() {
    global current_menu_entry_index

    SendEvent "{S}"
    current_menu_entry_index++
}

browse_to_menu(tab_name, entry) {
    SendEvent "{Tab}"
    Sleep 200

    to_tab := get_array_index(menu_tabs, tab_name)
    while current_menu_tab_index < to_tab {
        input_next_tab()
    }

    while current_menu_tab_index > to_tab {
        input_previous_tab()
    }

    to_entry := get_array_index(menu_entries[tab_name], entry)
    while current_menu_entry_index < to_entry {
        input_next_entry()
    }

    while current_menu_entry_index > to_entry {
        input_previous_entry()
    }
}

close_menu() {
    SendEvent "{Tab}"
    Sleep 200
}