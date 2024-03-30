#! /bin/bash

cd ~/OVCC && \
rm -f OVCC-1.6.1-arm64.dmg && \
cd ~/OVCC/CoCo/OVCC && \
create-dmg --window-pos 600 300 --icon-size 64 --background ~/OVCC/backgd4.jpg --icon ovcc.app 150 150 --app-drop-link 350 150 ../../OVCC-1.6.1-arm64.dmg . && \
cd ~/OVCC && \
rm -f OVCC-1.6.1-arm64.tar.gz && \
tar cvzf OVCC-1.6.1-arm64.tar.gz OVCC-1.6.1-arm64.dmg && \
rm -f OVCC-1.6.1-arm64.zip && \
zip -r OVCC-1.6.1-arm64.zip OVCC-1.6.1-arm64.dmg
