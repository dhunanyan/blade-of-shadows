#!/usr/bin/env bash
set -euo pipefail

SOURCE_DIR="${1:-assets/audio/music}"
OUTPUT_DIR="${2:-${SOURCE_DIR}/encoded}"

mkdir -p "${OUTPUT_DIR}"

encode_with_afconvert() {
  afconvert "$1" "$2" -f m4af -d aac -b 160000
}

encode_with_ffmpeg() {
  ffmpeg -hide_banner -loglevel error -y -i "$1" -c:a aac -b:a 160k "$2"
}

if command -v afconvert >/dev/null 2>&1; then
  encoder=encode_with_afconvert
elif command -v ffmpeg >/dev/null 2>&1; then
  encoder=encode_with_ffmpeg
else
  printf 'Install afconvert (macOS) or ffmpeg to encode music assets.\n' >&2
  exit 1
fi

for source in "${SOURCE_DIR}"/*.wav; do
  [ -e "${source}" ] || continue
  stem="$(basename "${source}" .wav)"
  target="$(printf '%s/%02d.m4a' "${OUTPUT_DIR}" "$((10#${stem}))")"
  if [ ! -f "${target}" ] || [ "${source}" -nt "${target}" ]; then
    printf 'Encoding %s\n' "${source}"
    "${encoder}" "${source}" "${target}"
  fi
done

printf 'Encoded music is available in %s\n' "${OUTPUT_DIR}"
