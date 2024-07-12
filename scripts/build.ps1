xmake f -m "Game__Test__Win64" -y
xmake -b "FrameMeterMod"

$game_path = "C:\Program Files (x86)\Steam\steamapps\common\Granblue Fantasy Versus Rising\RED\Binaries\Win64"
$mod_path = "$game_path\Mods\FrameMeterMod"
if (Test-Path $mod_path) {
	Remove-Item -Path $mod_path -Recurse
}

New-Item -Type dir -Force "$mod_path" | Out-Null
New-Item -Type file -Force "$mod_path\enabled.txt" | Out-Null
New-Item -Type dir -Force "$mod_path\dlls" | Out-Null
Copy-Item "Binaries\Game__Test__Win64\FrameMeterMod\FrameMeterMod.dll" -Destination "$mod_path\dlls\main.dll"
Copy-Item "Binaries\Game__Test__Win64\proxy\dwmapi.dll" -Destination "$game_path"
Copy-Item "Binaries\Game__Test__Win64\UE4SS\UE4SS.dll" -Destination "$game_path"