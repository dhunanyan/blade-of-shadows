#!/usr/bin/env sh
set -eu
set -o pipefail 2>/dev/null || true

BUILD_DIR="${1:-generated}"

CLR_RESET='\033[0m'
CLR_BOLD='\033[1m'
CLR_GREEN='\033[32m'
CLR_YELLOW='\033[33m'
CLR_RED='\033[31m'
CLR_BLUE='\033[34m'

log_ok()   { printf '%b\n' "${CLR_GREEN}${CLR_BOLD}[ OK ]${CLR_RESET} $*"; }
log_warn() { printf '%b\n' "${CLR_YELLOW}${CLR_BOLD}[WARN]${CLR_RESET} $*"; }
log_err()  { printf '%b\n' "${CLR_RED}${CLR_BOLD}[ERR ]${CLR_RESET} $*"; }
on_exit() {
  rc=$?
  if [ "${rc}" -ne 0 ]; then
    log_err "Komenda nie powiodła się."
  fi
}
trap on_exit EXIT

QT_PREFIX=""
if command -v brew >/dev/null 2>&1; then
  QT_CANDIDATE="$(brew --prefix qt 2>/dev/null || true)"
  if [ -n "${QT_CANDIDATE}" ] && [ -d "${QT_CANDIDATE}/lib/cmake" ]; then
    QT_PREFIX="${QT_CANDIDATE}"
  else
    QT_CANDIDATE="$(brew --prefix qt@6 2>/dev/null || true)"
    if [ -n "${QT_CANDIDATE}" ] && [ -d "${QT_CANDIDATE}/lib/cmake" ]; then
      QT_PREFIX="${QT_CANDIDATE}"
    fi
  fi
fi

if [ -n "${QT_PREFIX}" ]; then
  log_ok "Konfiguracja CMake z Qt prefix: ${CLR_BLUE}${QT_PREFIX}${CLR_RESET}"
  cmake -S . -B "${BUILD_DIR}" -DCMAKE_PREFIX_PATH="${QT_PREFIX}"
else
  log_warn "Nie znaleziono zainstalowanego Qt przez Homebrew (qt / qt@6). Konfiguracja bez CMAKE_PREFIX_PATH."
  cmake -S . -B "${BUILD_DIR}"
fi

log_ok "Budowanie targetu ${CLR_BLUE}game${CLR_RESET}"
if ! cmake --build "${BUILD_DIR}" --target game; then
  log_err "Nie udało się zbudować targetu 'game'. Sprawdź instalację Qt6."
  exit 1
fi

GAME_BIN="./${BUILD_DIR}/bin/game"
GAME_APP_BIN="./${BUILD_DIR}/bin/game.app/Contents/MacOS/game"

if [ -x "${GAME_BIN}" ]; then
  log_ok "Uruchamianie GUI ${CLR_BLUE}${GAME_BIN}${CLR_RESET}"
  "${GAME_BIN}"
elif [ -x "${GAME_APP_BIN}" ]; then
  log_ok "Uruchamianie GUI ${CLR_BLUE}${GAME_APP_BIN}${CLR_RESET}"
  "${GAME_APP_BIN}"
else
  log_err "Nie znaleziono ${GAME_BIN} ani ${GAME_APP_BIN}."
  exit 1
fi
