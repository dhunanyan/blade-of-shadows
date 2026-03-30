#!/usr/bin/env sh
set -eu
set -o pipefail 2>/dev/null || true

TEST_LIB_DIR="tests/lib"
GTEST_REPO_URL="https://github.com/google/googletest.git"

CLR_RESET='\033[0m'
CLR_BOLD='\033[1m'
CLR_GREEN='\033[32m'
CLR_YELLOW='\033[33m'
CLR_RED='\033[31m'
CLR_BLUE='\033[34m'

log_ok()   { printf '%b\n' "${CLR_GREEN}${CLR_BOLD}[ OK ]${CLR_RESET} $*"; }
log_warn() { printf '%b\n' "${CLR_YELLOW}${CLR_BOLD}[ WARN ]${CLR_RESET} $*"; }
log_err()  { printf '%b\n' "${CLR_RED}${CLR_BOLD}[ ERR ]${CLR_RESET} $*"; }

if [ -d "${TEST_LIB_DIR}/.git" ]; then
  log_ok "Test library already exists: ${CLR_BLUE}${TEST_LIB_DIR}${CLR_RESET}"
  exit 0
fi

if [ -d "${TEST_LIB_DIR}" ]; then
  log_warn "Directory ${CLR_BLUE}${TEST_LIB_DIR}${CLR_RESET} exists and is not a git repository. Recreating it from GitHub."
  rm -rf "${TEST_LIB_DIR}"
fi

log_ok "Fetching test library into ${CLR_BLUE}${TEST_LIB_DIR}${CLR_RESET}"
git clone --depth 1 "${GTEST_REPO_URL}" "${TEST_LIB_DIR}"
