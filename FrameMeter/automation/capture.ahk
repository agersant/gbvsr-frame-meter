#include "config.ahk"

dump_file := game_directory "\dump.zip"
hitboxes_file := game_directory "\capture.hitboxes"
meter_file := game_directory "\capture.meter"

begin_capture() {
    SendEvent "{F8}"
}

clean_artifacts() {
    if FileExist(dump_file) {
        FileDelete dump_file
    }
    if FileExist(hitboxes_file)
    {
        FileDelete hitboxes_file
    }
    if FileExist(meter_file)
    {
        FileDelete meter_file
    }
    if DirExist(output_directory) {
        DirDelete output_directory, 1
    }
    DirCreate output_directory
}

wait_for_file(name) {
    while !FileExist(name) {
        Sleep 50
    }
}

move_file_with_retries(from, to) {
    loop {
        try FileMove(from, to)
        catch Error {
            Sleep 100
            continue
        }
        break
    }
}

save_test_result(test_name) {
    wait_for_file(hitboxes_file)
    wait_for_file(meter_file)
    wait_for_file(dump_file)
    move_file_with_retries(dump_file, output_directory "\" test_name ".zip")
    move_file_with_retries(hitboxes_file, output_directory "\" test_name ".hitboxes")
    move_file_with_retries(meter_file, output_directory "\" test_name ".meter")
}