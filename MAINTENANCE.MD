# Setup

In the game folder, update the following settings in UE4SS-settings.ini:

```
GuiConsoleEnabled = 1
GuiConsoleVisible = 1
```

Execute `.scripts\build.ps1`

# Updating version check

Launch the game.

Console window should have an error message somewhere that looks like:
`FrameMeterMod failed game version check. Expected 'Version:2024/03/27 Revision:43575' but found 'Version:2024/03/28 Revision:43684'.` The found version part is likely to be nonsense.

Attach Cheat Engine to the game and click `Memory View`, then `View -> All Strings`. Check `Strings must match reg exp` and `Case Sensitive`, then type in `Revision` in the search field. Click `Generate String Map` and then `Show List`.

Find the string containing what looks like the latest game version number and take note of its address. In the main CE window, click `Add Address Manually` and enter the string address. Select the newly added address and press `Ctrl+B` to open the memory viewer. In your calculator of choice, compute the difference from the string address and the `Allocation Base` listed in memory viewer. For example,`0x7FF78831B800 - 0x7FF783D90000 = 0x458B800`. Update `check_game_version` in `dllmain.cpp` to use this offset.

Close the game and cheat engine, run `.scripts\build.ps1` again and re-launch the game. Version check error message should be gone.

# Updating battle hooks

After major game updates, some of the following problems are likely:

- Game crashes when entering training
- Meter stays blank
- Meter does not clear itself when resetting training (most common)

If the game crashes, one possible cause is that the `battle` field offset in `AREDGameState_Battle` needs to be updated.
Other issues may indicate that the `update_battle` or `reset_battle` function signatures need updating (in `on_unreal_init` within `dllmain.cpp`).

## Finding offset of `battle` field

TODO

## Finding update_battle signature

TODO

## Finding reset_battle signature

TODO

# Updating and running the test suite

Delete all the `.zip` files in `.\FrameMeter\test_data` - you are about to re-generate all of these.

For each `.meter` file, you need to record a memory dump of a specific gameplay sequence. The name of the files list out what moves to perform, while their content shows expected meter results. To record a sequence, press `F8` and then perform the sequence. Recording starts automatically on the first non-blank frame meter frame, and ends when both characters have a blank frame. The resulting recording shows up as `dump.zip` in the game folder. For sequences that specify `guard`, perform those in the corner position. For the `whiff` or `ranged` sequences, use the training neutral position (except for Katalina `2S+U` which requires more runway).

Replace all the deleted `.zip` files with your new recordings and run `.scripts\test.ps1`.

For failed tests, make sure the recording is correct and/or actually work on the mod logic to fix problems (😭).

# Misc functionality to validate

- Watch a replay and make sure the frame meter disappears during match intro/outro
- Press F4 to toggle meter on/off

# Publishing release

After pushing all the changes to Github and making sure CI passes, run the `Make Release` job and write release notes when it's done.
