#!/usr/bin/env python3

import math
import struct
import wave
from pathlib import Path


SAMPLE_RATE = 22050
OUTPUT_DIR = Path(__file__).resolve().parents[1] / "assets" / "audio" / "sfx"


def write_tone(name: str, frequencies: list[float], duration: float, volume: float = 0.35) -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    frame_count = int(SAMPLE_RATE * duration)
    attack = max(1, int(frame_count * 0.08))
    release = max(1, int(frame_count * 0.28))

    with wave.open(str(OUTPUT_DIR / name), "wb") as output:
        output.setnchannels(1)
        output.setsampwidth(2)
        output.setframerate(SAMPLE_RATE)

        frames = bytearray()
        for index in range(frame_count):
            progress = index / max(1, frame_count - 1)
            frequency_index = min(int(progress * len(frequencies)), len(frequencies) - 1)
            frequency = frequencies[frequency_index]
            envelope = min(1.0, index / attack, (frame_count - index) / release)
            sample = math.sin(2.0 * math.pi * frequency * index / SAMPLE_RATE)
            frames.extend(struct.pack("<h", int(sample * envelope * volume * 32767)))
        output.writeframes(frames)


def main() -> None:
    write_tone("attack.wav", [260.0, 190.0, 130.0], 0.13)
    write_tone("jump.wav", [330.0, 440.0, 590.0], 0.16)
    write_tone("coin.wav", [660.0, 880.0, 1100.0], 0.18)
    write_tone("hurt.wav", [180.0, 120.0, 85.0], 0.22, 0.45)


if __name__ == "__main__":
    main()
