# Audio

## Music Playback

`MusicPlayer` owns `QMediaPlayer`, `QAudioOutput`, the soundtrack catalog,
shuffle state, and playback history. It:

- starts on a random track;
- plays every remaining track in shuffled order before refilling the shuffle bag;
- automatically advances when a track ends;
- supports history-aware Previous and Next actions;
- exposes the current display name to the settings menu;
- stops completely when music is disabled;
- resumes the selected track when music is enabled.

The numeric WAV files in `assets/audio/music` are local lossless source
masters. They are ignored by Git so the repository does not carry roughly
1.5 GB of uncompressed runtime-equivalent audio. Production AAC files live in
`assets/audio/music/encoded`, are versioned with the game, and can be
regenerated with:

```bash
./scripts/prepare_music_assets.sh
```

The script uses `afconvert` on macOS or `ffmpeg` on other platforms.

## Soundtrack

| ID | Display name |
| --- | --- |
| 01 | Moonlit Oath |
| 02 | Ashen Footsteps |
| 03 | Lanterns in the Fog |
| 04 | Silent Katana |
| 05 | Echoes Beneath Stone |
| 06 | The Long Night March |
| 07 | Shuriken Rain |
| 08 | Crimson Canopy |
| 09 | Hidden Path |
| 10 | Shadowborne |
| 11 | Temple of Broken Bells |
| 12 | Frost on the Blade |
| 13 | Dusk Runner |
| 14 | Hollow Sanctum |
| 15 | Ember Veil |
| 16 | Whispering Steel |
| 17 | Ronin's Resolve |
| 18 | Cavern Pulse |
| 19 | Blood Moon Crossing |
| 20 | Bamboo After Dark |
| 21 | Spectral Pursuit |
| 22 | Shrine of Cinders |
| 23 | Nocturne of the Lost |
| 24 | Warden of Mist |
| 25 | Black Feather Waltz |
| 26 | Thunder Without Sky |
| 27 | Veins of the Mountain |
| 28 | Last Light at the Gate |
| 29 | Phantom Province |
| 30 | Blades in Autumn |
| 31 | The Unseen Road |
| 32 | Iron Lotus |
| 33 | Nightfall Rebellion |
| 34 | Beneath Violet Clouds |
| 35 | Wolf at the Torii |
| 36 | Siege of Silence |
| 37 | Darkwater Reflection |
| 38 | Oathkeeper's Descent |
| 39 | Bells Before Battle |
| 40 | Crown of Shadows |
| 41 | A Thousand Quiet Steps |
| 42 | The Final Lantern |
| 43 | Kingdom Under Eclipse |
| 44 | Dawn Through Smoke |
| 45 | Home of the Wandering Blade |
| 46 | Blade of Shadows |

## Sound Effects

All files under `assets/audio/sfx` are registered as Qt resources. Current
gameplay mappings are:

| Event | Resource |
| --- | --- |
| Attack | `shoot-2.wav` |
| Jump | `jump-1.wav` |
| Double jump | `powerup-1.wav` |
| Coin collected | `coin-1.wav` |
| Player damaged | `damage-1.wav` |
| Player defeated | `die-1.wav` |
| Level completed | `level-complete-1.wav` |

The remaining effects are available for future checkpoints, destructible
objects, alternate variants, switches, and ranged attacks.
