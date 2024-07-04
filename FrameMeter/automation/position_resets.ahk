reset_to_midscreen() {
    Send "{S down}{1 down}"
    Sleep 50
    Send "{S up}{1 up}"
    Sleep 1000
}

reset_to_left_corner() {
    Send "{A down}{1 down}"
    Sleep 50
    Send "{A up}{1 up}"
    Sleep 1000
}

reset_to_right_corner() {
    Send "{D down}{1 down}"
    Sleep 50
    Send "{D up}{1 up}"
    Sleep 1000
}