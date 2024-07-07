#include "config.ahk"

moves := Map()

moves["evade"] := [key_block, key_move_left]

moves["cL"] := [key_light]
moves["fL"] := [key_light]
moves["L"] := [key_light]
moves["cM"] := [key_medium]
moves["fM"] := [key_medium]
moves["M"] := [key_medium]
moves["cH"] := [key_heavy]
moves["fH"] := [key_heavy]
moves["H"] := [key_heavy]
moves["U"] := [key_unique]
moves["Throw"] := [key_throw]

moves["6+H"] := [key_move_right, key_heavy]

moves["S"] := [key_skill]
moves["S+M"] := [key_skill, key_medium]
moves["S+H"] := [key_skill, key_heavy]
moves["S+U"] := [key_skill, key_unique]

moves["4S"] := [key_move_left, key_skill]
moves["4S+M"] := [key_move_left, key_skill, key_medium]
moves["4S+H"] := [key_move_left, key_skill, key_heavy]
moves["4S+U"] := [key_move_left, key_skill, key_unique]

moves["2S"] := [key_move_down, key_skill]
moves["2S+M"] := [key_move_down, key_skill, key_medium]
moves["2S+H"] := [key_move_down, key_skill, key_heavy]
moves["2S+U"] := [key_move_down, key_skill, key_unique]

moves["6S"] := [key_move_right, key_skill]
moves["6S+M"] := [key_move_right, key_skill, key_medium]
moves["6S+H"] := [key_move_right, key_skill, key_heavy]
moves["6S+U"] := [key_move_right, key_skill, key_unique]

sleep_frames(num_frames) {
    Sleep num_frames * 1000 / 60
}

do_move(move) {
    for index, key in moves[move] {
        Send "{" key " down}"
    }
    sleep 50
    for index, key in moves[move] {
        Send "{" key " up}"
    }
}