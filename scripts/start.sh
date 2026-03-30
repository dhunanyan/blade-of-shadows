#!/usr/bin/env sh
set -eu
set -o pipefail 2>/dev/null || true

BUILD_DIR="${1:-build}"

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
    log_err "Command failed."
  fi
}
trap on_exit EXIT

GAME_BIN="./${BUILD_DIR}/bin/game"
GAME_APP_BIN="./${BUILD_DIR}/bin/game.app/Contents/MacOS/game"

if [ -x "${GAME_BIN}" ]; then
  log_ok "Launching GUI ${CLR_BLUE}${GAME_BIN}${CLR_RESET}"
  "${GAME_BIN}"
elif [ -x "${GAME_APP_BIN}" ]; then
  log_ok "Launching GUI ${CLR_BLUE}${GAME_APP_BIN}${CLR_RESET}"
  "${GAME_APP_BIN}"
else
  log_err "Could not find ${GAME_BIN} or ${GAME_APP_BIN}. Run ./scripts/build.sh ${BUILD_DIR} first."
  exit 1
fi
