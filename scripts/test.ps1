cmake --build output --target FrameMeterTest --config Game__Test__Win64

if ($LastExitCode -ne 0) {
	exit $LastExitCode;
}

./output/FrameMeter/Game__Test__Win64/FrameMeterTest.exe @args
