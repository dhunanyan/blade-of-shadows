#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${1:-build-release}"
DIST_DIR="${2:-dist}"

"$(dirname "$0")/build.sh" "${BUILD_DIR}"
rm -rf "${DIST_DIR}"
mkdir -p "${DIST_DIR}"

if [ -d "${BUILD_DIR}/bin/BladeOfShadows.app" ]; then
  APP_PATH="${BUILD_DIR}/bin/BladeOfShadows.app"
elif [ -d "${BUILD_DIR}/bin/game.app" ]; then
  APP_PATH="${BUILD_DIR}/bin/game.app"
else
  APP_PATH=""
fi

if [ -n "${APP_PATH}" ] && command -v macdeployqt >/dev/null 2>&1; then
  macdeployqt "${APP_PATH}" -no-plugins

  QT_PLUGIN_DIR="$(
    qtpaths6 --query QT_INSTALL_PLUGINS 2>/dev/null ||
      qtpaths --query QT_INSTALL_PLUGINS 2>/dev/null
  )"
  declare -a REQUIRED_PLUGINS=(
    "platforms/libqcocoa.dylib"
    "multimedia/libdarwinmediaplugin.dylib"
    "styles/libqmacstyle.dylib"
  )

  for relative_plugin in "${REQUIRED_PLUGINS[@]}"; do
    source_plugin="${QT_PLUGIN_DIR}/${relative_plugin}"
    target_plugin="${APP_PATH}/Contents/PlugIns/${relative_plugin}"
    if [ ! -f "${source_plugin}" ]; then
      printf 'Required Qt plugin not found: %s\n' "${source_plugin}" >&2
      exit 1
    fi

    mkdir -p "$(dirname "${target_plugin}")"
    cp "${source_plugin}" "${target_plugin}"

    while IFS= read -r dependency; do
      framework_path="$(sed -E 's#^.*(/Qt[^/]+\.framework/.*)$#\1#' <<<"${dependency}")"
      install_name_tool -change \
        "${dependency}" \
        "@executable_path/../Frameworks${framework_path}" \
        "${target_plugin}"
    done < <(
      otool -L "${target_plugin}" |
        tail -n +2 |
        awk '{print $1}' |
        grep -E '(^@rpath|^/opt/homebrew).*/?Qt[^/]+\.framework/' || true
    )
  done

  while IFS= read -r binary; do
    if ! file "${binary}" | grep -q 'Mach-O'; then
      continue
    fi

    install_name="$(otool -D "${binary}" 2>/dev/null | sed -n '2p')"
    while IFS= read -r dependency; do
      if [ "${dependency}" = "${install_name}" ]; then
        continue
      fi
      if grep -Eq '/opt/homebrew|@rpath/Qt(Pdf|Svg|VirtualKeyboard)' <<<"${dependency}"; then
        printf 'Unresolved deployment dependency in %s: %s\n' \
          "${binary}" "${dependency}" >&2
        exit 1
      fi
    done < <(otool -L "${binary}" | tail -n +2 | awk '{print $1}')
  done < <(find "${APP_PATH}/Contents" -type f)

  CODESIGN_IDENTITY="${CODESIGN_IDENTITY:--}"
  if [ "${CODESIGN_IDENTITY}" = "-" ]; then
    codesign --force --deep --sign - "${APP_PATH}"
  else
    codesign --force --deep --options runtime --timestamp \
      --sign "${CODESIGN_IDENTITY}" "${APP_PATH}"
  fi
  codesign --verify --deep --strict "${APP_PATH}"

  if command -v hdiutil >/dev/null 2>&1; then
    DMG_STAGING_DIR="${BUILD_DIR}/dmg-staging"
    rm -rf "${DMG_STAGING_DIR}"
    mkdir -p "${DMG_STAGING_DIR}"
    cp -R "${APP_PATH}" "${DMG_STAGING_DIR}/BladeOfShadows.app"
    ln -s /Applications "${DMG_STAGING_DIR}/Applications"
    hdiutil create \
      -volname "Blade of Shadows" \
      -srcfolder "${DMG_STAGING_DIR}" \
      -ov \
      -format UDZO \
      "${DIST_DIR}/BladeOfShadows.dmg"
  else
    cp -R "${APP_PATH}" "${DIST_DIR}/BladeOfShadows.app"
  fi
elif [ -n "${APP_PATH}" ]; then
  cp -R "${APP_PATH}" "${DIST_DIR}/BladeOfShadows.app"
else
  cmake --install "${BUILD_DIR}" --prefix "${DIST_DIR}/install"
  cmake -E tar cf "${DIST_DIR}/blade-of-shadows-${BUILD_DIR}.tar.gz" \
    --format=gnutar "${DIST_DIR}/install"
fi

printf 'Package output: %s\n' "${DIST_DIR}"
