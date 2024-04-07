#! /bin/bash

cd ~/OVCC && \
version=$(cat CoCo/AGARInterface.c|grep 'agwin,[ 	]*"OVCC'|cut -f2 -d\"|cut -f2 -d' ') && \
rm -f OVCC-${version}-arm64.dmg && \
cd ~/OVCC/CoCo/Debug && \
create-dmg --volname OVCC --volicon Ovcc.app/Contents/Resources/AppIcon.icns --window-pos 600 300 --icon-size 64 --background ~/OVCC/backgd4.jpg --icon Ovcc.app 150 150 --app-drop-link 350 150 ../../OVCC-${version}-arm64.dmg . && \
cd ~/OVCC && \
rm -f OVCC-${version}-arm64.tar.gz && \
tar cvzf OVCC-${version}-arm64.tar.gz OVCC-${version}-arm64.dmg && \
rm -f OVCC-${version}-arm64.zip && \
zip -r OVCC-${version}-arm64.zip OVCC-${version}-arm64.dmg
