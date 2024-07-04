reset_to_midscreen() {
    send "{S down}{1 down}"
    sleep 40
    send "{S up}{1 up}"
    sleep 1000
}

reset_to_left_corner() {
    send "{A down}{1 down}"
    sleep 40
    send "{A up}{1 up}"
    sleep 1000
}

reset_to_right_corner() {
    send "{D down}{1 down}"
    sleep 40
    send "{D up}{1 up}"
    sleep 1000
}