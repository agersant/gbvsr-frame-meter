#include "config.ahk"

moves := Map()
moves["S+U"] := [key_skill, key_unique]

do_move(move) {
    for index, key in moves[move] {
        send "{" key " down}"
    }
    sleep 50
    for index, key in moves[move] {
        send "{" key " up}"
    }
}