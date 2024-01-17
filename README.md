# GBVSR Frame Meter

This project is a mod for `Granblue Fantasy Versus: Rising`. It adds an on-screen display of character frame data during training and replays, similar to the frame meter from `Street Fighter 6`. This can be used to easily analyze gameplay situations, find frametraps or troubleshoot combos.

https://github.com/agersant/gbvsr-frame-meter/assets/817256/4fa609c1-3383-4f92-ac72-cfc3e0756ec5

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
| <img src="https://placehold.co/21x36/FFF830/FFF830" /> | Post-Damage/Post-Block Recovery Period |
| <img src="https://placehold.co/21x36/F1F1F0/F1F1F0" /> | Partial or Full Invincibility          |

## Keyboard Shortcuts

| Key  | Effect                    |
| :--: | ------------------------- |
| `F4` | Toggles meter visibility. |

## Caveats

- This mod is not compatible with the free version of the game.
- ⚠ This mod will stop working after every game update. Please come back to this page and download a new version after each game patch.
- If you run into any issues, you can disable the mod entirely it by renaming `enabled.txt` to `disabled.txt`. The location of this file is `C:\Program Files (x86)\Steam\steamapps\common\Granblue Fantasy Versus Rising\RED\Binaries\Win64\Mods\FrameMeterMod\enabled.txt`.
- There is currently no indication of parry/armor states like Charlotta's `5U` or Siegfried's `4S+U`.

## Credits

- Thanks to Ryn (@WistfulHopes) for her help and prior work on Arc System Works game modding.
- Thanks to the RE-UE4SS project for providing a useful modding framework.
