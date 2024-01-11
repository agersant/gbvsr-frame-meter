MSBuild.exe .\output\MyMods.sln -target:FrameMeterTest /p:Configuration=Game__Shipping__Win64

if ($LastExitCode -ne 0) {
	exit $LastExitCode;
}

./output/FrameMeter/Game__Shipping__Win64/FrameMeterTest.exe
