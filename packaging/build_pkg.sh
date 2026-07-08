#!/usr/bin/env bash
# Builds a macOS installer .pkg that places the EFFIGY VST3 + AU into the
# standard user/system plugin folders, then zips it. Run on macOS after the
# CMake build has produced the .vst3 and .component bundles.
set -euo pipefail

VERSION="${1:-1.0.0}"
BUILD_DIR="${2:-build}"
OUT_DIR="${3:-dist}"

VST3_SRC="${BUILD_DIR}/EFFIGY_artefacts/Release/VST3/EFFIGY.vst3"
AU_SRC="${BUILD_DIR}/EFFIGY_artefacts/Release/AU/EFFIGY.component"

# Fallback for non-Release configs
[ -d "$VST3_SRC" ] || VST3_SRC="$(find "$BUILD_DIR" -name 'EFFIGY.vst3' -type d | head -n1)"
[ -d "$AU_SRC" ]   || AU_SRC="$(find "$BUILD_DIR" -name 'EFFIGY.component' -type d | head -n1)"

echo "VST3: $VST3_SRC"
echo "AU:   $AU_SRC"

STAGE="$(mktemp -d)"
ROOT_VST3="${STAGE}/root/Library/Audio/Plug-Ins/VST3"
ROOT_AU="${STAGE}/root/Library/Audio/Plug-Ins/Components"
mkdir -p "$ROOT_VST3" "$ROOT_AU"

[ -d "$VST3_SRC" ] && cp -R "$VST3_SRC" "$ROOT_VST3/"
[ -d "$AU_SRC" ]   && cp -R "$AU_SRC"   "$ROOT_AU/"

mkdir -p "$OUT_DIR"
PKG="${OUT_DIR}/EFFIGY-${VERSION}.pkg"

pkgbuild \
  --root "${STAGE}/root" \
  --identifier "com.effigyaudio.effigy" \
  --version "${VERSION}" \
  --install-location "/" \
  "$PKG"

ZIP="${OUT_DIR}/EFFIGY-macOS-${VERSION}.zip"
( cd "$OUT_DIR" && zip -r "$(basename "$ZIP")" "$(basename "$PKG")" )
echo "Created $ZIP"
rm -rf "$STAGE"
