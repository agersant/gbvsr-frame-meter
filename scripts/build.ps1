MSBuild.exe .\output\MyMods.sln

$install_path = "C:\Program Files (x86)\Steam\steamapps\common\Granblue Fantasy Versus Rising\RED\Binaries\Win64\Mods\FrameMeterMod"
if (Test-Path $install_path) {
	Remove-Item -Path $install_path -Recurse
}

New-Item -Type dir -Force "$install_path" | Out-Null
New-Item -Type file -Force "$install_path\enabled.txt" | Out-Null
New-Item -Type dir -Force "$install_path\dlls" | Out-Null
Copy-Item "output/FrameMeterMod/Game__Debug__Win64/FrameMeterMod.dll" -Destination "$install_path\dlls"