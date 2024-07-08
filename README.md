# GBVSR Frame Meter

This project is a mod for `Granblue Fantasy Versus: Rising`. It adds an on-screen display of character frame data during training and replays, similar to the frame meter from `Street Fighter 6`. This can be used to easily analyze gameplay situations, find frametraps or troubleshoot combos.

https://github.com/agersant/gbvsr-frame-meter/assets/817256/01df62a7-afda-4ff5-948c-960ae2df3a15

## Installation

1. Go to the [latest release](https://github.com/agersant/gbvsr-frame-meter/releases/latest) page.
2. Download the `gbvsr-frame-meter.zip` archive.
3. Extract the archive content alongside your `GBVSR-Win64-Shipping.exe` file. Its default location is `C:\Program Files (x86)\Steam\steamapps\common\Granblue Fantasy Versus Rising\RED\Binaries\Win64`.
4. The resulting directory structure should look like this:

```
      📁 Granblue Fantasy Versus Rising\RED\Binaries
        📁 Win64
          📄 GBVSR-Win64-Shipping.exe
          📄 dwmapi.dll
          📄 UE4SS.dll
          📄 UE4SS-settings.ini
          📁 Mods
            📁 FrameMeterMod
              📄 enabled.txt
              📁 dlls
                📄 main.dll
```

5. Launch the game via Steam as usual.

## Understanding the Meter

|                         Color                          | Meaning                                |
| :----------------------------------------------------: | -------------------------------------- |
| <img src="https://placehold.co/21x36/00B796/00B796" /> | Counter State                          |
| <img src="https://placehold.co/21x36/0170BE/0170BE" /> | Recovery                               |
| <img src="https://placehold.co/21x36/42F8FD/42F8FD" /> | Non-Counter Action                     |
| <img src="https://placehold.co/21x36/CC2B67/CC2B67" /> | Hitbox Appearance Period               |
| <img src="https://placehold.co/21x36/C98002/C98002" /> | Projectile's Active Time               |
| <img src="https://placehold.co/21x36/5C1E6E/5C1E6E" /> | Armor or Parry                         |
| <img src="https://placehold.co/21x36/FFF830/FFF830" /> | Post-Damage/Post-Block Recovery Period |
| <img src="https://placehold.co/21x36/F1F1F0/F1F1F0" /> | Partial or Full Invincibility          |

## Understanding Hitboxes

|                         Color                          | Meaning                                                                                                                                                                                |
| :----------------------------------------------------: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| <img src="https://placehold.co/32x32/AAAAAA/AAAAAA" /> | These are collision boxes that make characters push each other.                                                                                                                        |
| <img src="https://placehold.co/32x32/00FF00/00FF00" /> | These boxes represent where characters can get hit by strikes.                                                                                                                         |
| <img src="https://placehold.co/32x32/FF0000/FF0000" /> | Strike hitboxes.                                                                                                                                                                       |
| <img src="https://placehold.co/32x32/FF00FF/FF00FF" /> | A simplified display of throw and command grab hitboxes. Grabs connect when this overlaps the center of the opponent pushbox (<img src="https://placehold.co/16x16/AAAAAA/AAAAAA" />). |

## Keyboard Shortcuts

| Key  | Effect                                                                                                 |
| :--: | ------------------------------------------------------------------------------------------------------ |
| `F3` | Cycle hitboxes visibility between `None` / `All` / `Only Player 1` / `Only Player 2 `(default `None`). |
| `F4` | Toggles frame meter visibility (default `On`).                                                         |

## Caveats

- This mod is not compatible with the free version of the game.
- ⚠ This mod will stop working after every game update. Please come back to this page and download a new version after each game patch.
- If you run into any issues, you can disable the mod entirely it by renaming `enabled.txt` to `disabled.txt`. The location of this file is `C:\Program Files (x86)\Steam\steamapps\common\Granblue Fantasy Versus Rising\RED\Binaries\Win64\Mods\FrameMeterMod\enabled.txt`.

## Credits

- Thanks to Ryn (@WistfulHopes) for her help and prior work on Arc System Works game modding.
- Thanks to the RE-UE4SS project for providing a useful modding framework.
