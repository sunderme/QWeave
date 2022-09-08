#!/usr/bin/env sh

# Exit on errors
set -e
VERSION_NAME=`git describe`
echo "package build into zip for win"
# workaround for botched qt6 installation
mkdir -p package-zip
cp qweave.exe package-zip/
cp -r translation package-zip/translations
cd package-zip
windeployqt-qt6 qweave.exe
ldd qweave.exe | awk '{print $3}'| grep ming | xargs -I{} cp -u {} .
cd ..
echo "make installer"
cp ../resources/qweave-msys.nsi .
cp ../resources/FileAssociation.nsh .
makensis qweave-msys.nsi

cd package-zip
zip -r ./qweave-win-qt6-${VERSION_NAME}.zip *

cd ..
sha256sum ./package-zip/qweave-win-qt6-${VERSION_NAME}.zip
cp ./package-zip/qweave-win-qt6-${VERSION_NAME}.zip ./qdia-${VERSION_NAME}-win-portable-qt6.zip
cp ./qweave_installer.exe ./qweave-win-qt6-${VERSION_NAME}.exe


