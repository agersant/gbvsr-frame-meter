MSBuild.exe .\output\MyMods.sln -target:proxy /p:Configuration=Game__Shipping__Win64
MSBuild.exe .\output\MyMods.sln -target:FrameMeterMod /p:Configuration=Game__Shipping__Win64

$game_path = "C:\Program Files (x86)\Steam\steamapps\common\Granblue Fantasy Versus Rising\RED\Binaries\Win64"
$mod_path = "$game_path\Mods\FrameMeterMod"
if (Test-Path $mod_path) {
	Remove-Item -Path $mod_path -Recurse
}

New-Item -Type dir -Force "$mod_path" | Out-Null
New-Item -Type file -Force "$mod_path\enabled.txt" | Out-Null
New-Item -Type dir -Force "$mod_path\dlls" | Out-Null
Copy-Item "output/FrameMeter/Game__Shipping__Win64/FrameMeterMod.dll" -Destination "$mod_path\dlls\main.dll"
Copy-Item "output/Output/Game__Shipping__Win64/proxy/bin/dwmapi.dll" -Destination "$game_path"
Copy-Item "output/Output/Game__Shipping__Win64/UE4SS/bin/UE4SS.dll" -Destination "$game_path"