#include "menu_usage.ahk"
#include "utils.ahk"

character_list := [
    "gran", "djeeta", "katalina", "charlotta", "lancelot", "percival", "ladiva", "metera",
    "lowain", "ferry", "zeta", "vaseraga", "narmaya", "soriz", "zooey", "cagliostro",
    "yuel", "anre", "eustace", "seox", "vira", "anila", "siegfried", "grimnir",
    "nier", "beelzebub", "belial", "avatar_belial", "lucilius", "2b", "vane", "beatrix"
]

current_character := "gran"

character_to_grid_position(character_name) {
    row_width := 8
    index := get_array_index(character_list, character_name)
    x := Mod(index - 1, row_width)
    y := (index - 1) // row_width
    return [x, y]
}

select_character(character_name) {
    global current_character

    if character_name == current_character {
        return
    }

    browse_to_menu("quick_menu", "character_1p")
    SendEvent "{Enter}"
    Sleep 200

    from := character_to_grid_position(current_character)
    to := character_to_grid_position(character_name)

    while from[1] < to[1] {
        from[1]++
        SendEvent "{D}"
    }

    while from[1] > to[1] {
        from[1]--
        SendEvent "{A}"
    }

    while from[2] < to[2] {
        from[2]++
        SendEvent "{S}"
    }

    while from[2] > to[2] {
        from[2]--
        SendEvent "{W}"
    }

    SendEvent "{Enter}"
    Sleep 300

    close_menu()
    Sleep 5000

    current_character := character_name
}