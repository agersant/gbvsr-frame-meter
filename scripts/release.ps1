if (Test-Path "gbvsr-frame-meter.zip") {
	Remove-Item -Path "gbvsr-frame-meter.zip"
}

$game_path = "release"
if (Test-Path "$game_path") {
	Remove-Item -Path "$game_path" -Recurse
}

xmake f -m "Game__Shipping__Win64" -y
xmake -b "FrameMeterMod"

New-Item -Type dir -Force "$game_path" | Out-Null
Copy-Item -Path "Binaries\Game__Shipping__Win64\proxy\dwmapi.dll" -Destination "$game_path"
Copy-Item -Path "Binaries\Game__Shipping__Win64\UE4SS\UE4SS.dll" -Destination "$game_path"
Copy-Item -Path "RE-UE4SS\assets\UE4SS-settings.ini" -Destination "$game_path"

$mod_path = "$game_path\Mods\FrameMeterMod"
New-Item -Type dir -Force "$mod_path" | Out-Null
New-Item -Type dir -Force "$mod_path/dlls" | Out-Null
Copy-Item -Path "Binaries/Game__Shipping__Win64\FrameMeterMod\FrameMeterMod.dll" -Destination "$mod_path/dlls/main.dll"
New-Item -Type file -Force "$mod_path\enabled.txt" | Out-Null

$settings = get-content "RE-UE4SS\assets\UE4SS-settings.ini"
$settings = $settings -replace "ConsoleEnabled = 1", "ConsoleEnabled = 0"
$settings = $settings -replace "GuiConsoleEnabled = 1", "GuiConsoleEnabled = 0"
$settings = $settings -replace "bUseUObjectArrayCache = true", "bUseUObjectArrayCache = false"
$settings > "$game_path\UE4SS-settings.ini"

Compress-Archive -Path "$game_path\*" -DestinationPath "gbvsr-frame-meter.zip"
