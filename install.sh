#!/bin/sh

./.installreq.sh
./.buildurho.sh

git pull
cd ..
mkdir KO-build
cd KO-build
qmake ../KO/KO.pro
sudo make install
sudo chown -R $USER ~/.local/share/luckey/ko/
sudo chown $USER ~/.local/share/icons/ko.svg
update-icon-caches ~/.local/share/icons/
cd ..
rm -rf KO-build
