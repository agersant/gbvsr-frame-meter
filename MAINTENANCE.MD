# Setup

Install Cheat Engine and Autohotkey. In Cheat Engine settings, go to `Debugger Options` and select `Use VEH Debugger`.

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

# Supporting the new game version

After major game updates, various maintenance task may be required:

## Troubleshooting crashes

### What this fixes

- Game crashes when entering training or performing moves

### How to

- Navigate to `%LocalAppData%\GBVSR\Saved\Crashes`.
- Open the most recent directory.
- Use `WinDbg` to open `UE4Minidump.dmp`.
- Execute `!analyze -v`.
- Scroll down to see `FAULTING_SOURCE_FILE` and `FAULTING_SOURCE_LINE_NUMBER`.
- This will most likely point you to somewhere in the source of this mod.
- Most likely the mod is trying to dereference a pointer, but the offset of the pointer field is incorrect.
- Good luck from here!

### Common crash causes:

- The offset of the `->battle` field in `AREDGameState_Battle` needs to be updated.

## Updating `update_battle` signature

### What this fixes

- Meter stays blank during combat

### How to

- Attach cheat engine.
- Click the `Memory View` button.
- In the `Memory View` window, click on `View` -> `All Strings` in the top menu bar.
- Enable case sensitive search and regex matching.
- Type in the string `cmn_hit_s` and click `Generate String Map`, then `Show List`.
- In the `Address List`, manually add the address of the string you find.
- Select the newly added address in the address list and press Ctrl+B to inspect this memory location.
- Right click on the first letter of the string and select `Data Breakpoint` -> `Break on Access`.
- Hit the training dummy with a close light attack (make sure the dummy is setup not to block it)
- Your breakpoint should hit.
- Double click entries in the callstack one at a time.
- For each entry, right click the instruction that was jumped to and click `Select Current Function`.
- Compare the compiled assembly of this function with the signature in `update_battle_detour` within `dllmain.cpp`. Keeping going until you find a function that is _almost_ identical to the known signature.
- Usually, the correct one entry is the 9th from top of the stack. Functions 1-9 have similar offsets (while #10 is somewhere completely different in the program).
- Update the signature in `dllmain.cpp` to match the one you found.

## Updating `reset_battle` signature

### What this fixes

- Frame meter does not reset when training is reset
- Important debug output (starting with `Battle Reset. Game Instance:`) is missing from the console.

### How to

- Attach cheat engine
- Enable speedhack 0.1 in the main Cheat Engine window
- In the nearby memory scan options, set `Value` type to `String`
- Type `Bluemoon` for value. In training, have Charlotta perform `Shining Onslaught` (any version). While the move is in progress, click `First Scan` in Cheat Engine.
- Type `Hooligan` for value. In training, have Charlotta perform `Noble Strategy` (any version). While the move is in progress, click `First Scan` in Cheat Engine.
- The left panel should show only one address, right click it and add it to the address list.
- Select the newly added address in the address list and press Ctrl+B to inspect this memory location.
- Press ctrl+enter to switch to relative address mode.
- Scroll down to around +00009CA4 (that's pretty far down!).
- Try to spot a value this is `54` while holding the guard button, `85` while spot dodging, `00` while idle.
- Once you spot it, right click this byte and select `Data Breakpoint` -> `Break on Write`
- Reset training using the in-game dedicated button - your breakpoint should hit.
- Double click the top entry in the callstack
- Right click the instruction that was jumped to and pick `Select Current Function`
- Compare the compiled assembly of this function with the signature in `reset_battle_detour` within `dllmain.cpp`. They should be _almost_ identical.
- Update the signature in `dllmain.cpp` to match the one you found.

# Updating and running the test suite

Delete all the `.zip` files in `.\FrameMeter\test_data` - you are about to re-generate all of these. Make sure to keep the `.meter` and `.hitboxes` files as-is.

### Automated recording (preferred)

- Install [AutoHotkey](https://www.autohotkey.com/) and add it to your `PATH`
- Launch the game and start training mode as Gran VS Djeeta. Do not open the options menu.
- Ensure the button config in `.\FrameMeter\automation\config.ahk` matches your in-game settings.
- Run `.scripts\record_test_data.ps1`,
- This will automatically play out a number of moves and record memory dumps of the game state, as well as frame meter state and any hitboxes detected.
- When the recordings are complete (as of this writing, the last test is Zeta laser from range), copy the generated `.zip` files from `.\automation_output` to `.\FrameMeter\test_data`.

### Manual recording

For each `.meter` file in `.\FrameMeter\test_data`, you need to record a memory dump of a specific gameplay sequence. The name of the files list out what moves to perform, while their content shows expected meter results. To record a sequence, press `F8` and then perform the move. Recording starts automatically on the first non-blank frame meter frame, and ends when both characters have a blank frame. The resulting recording shows up as `dump.zip` in the game folder.

For sequences that specify `guard`, perform those in the corner position. For the `whiff` or `ranged` sequences, use the training default midscreen position. In doubt, you can also consult the test implementations in `.\FrameMeter\automation\tests.ahk`.

Replace all the deleted `.zip` files from `.\FrameMeter\test_data` with your new recordings.

### Running the test suite

Run `.scripts\test.ps1`.

For failed tests:

- Make sure the recording was done correctly.
- Update the affected `.meter` and/or `.hitboxes` files **if a game patch has adjusted relevant move properties**.
- Adjust the mod logic to fix possible problems (😭).

# Misc functionality to validate

- Watch a replay and make sure the frame meter disappears during match intro/outro.
- Press F4 to toggle meter on/off.
- Press F3 and make sure hurtboxes/hitboxes are drawing at all.

# Publishing release

After pushing all the changes to Github and making sure CI passes, run the `Make Release` job and write release notes when it's done.
