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

log_ok "Uruchamianie ${CLR_BLUE}./${BUILD_DIR}/bin/lab3Inheritance_forStudents${CLR_RESET}"
"./${BUILD_DIR}/bin/lab3Inheritance_forStudents"
