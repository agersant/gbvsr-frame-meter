xmake f -m "Game__Test__Win64" -y
xmake -b "FrameMeterTest"

if ($LastExitCode -ne 0) {
	exit $LastExitCode;
}

.\Binaries\Game__Test__Win64\FrameMeterTest\FrameMeterTest.exe @args
