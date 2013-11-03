#!/bin/bash
Program=usbavrlaboszi
Widgetset=$1
if [ "x$Widgetset" = "x" ]; then
  Widgetset=gtk2
fi
Arch=$(fpc -v | grep 'Compiler version' | sed 's/.*for \([^ ]\+\)$/\1/')
#'
Year=`date +%y`
Month=`date +%m`
Day=`date +%d`
Date=20$Year$Month$Day
TmpDir=/tmp
BuildDir=$TmpDir/software_build
Version=$(sed 's/\x0D$//' ../../source/version.inc).$(sed 's/\x0D$//' ../../source/revision.inc)
echo "Build directory is $BuildDir"
if [ x$BuildDir = x/ ]; then
  echo "ERROR: invalid build directory"
  exit
fi
rm -rf $BuildDir
echo "creating control file..."
mkdir -p $BuildDir/DEBIAN
cat debian/control | \
  sed -e "s/VERSION/$Version/g" \
      -e "s/ARCH/$Arch/g" \
  > $BuildDir/DEBIAN/control
echo "copyright and changelog files..."
mkdir -p $BuildDir/usr/share/doc/$Program
cp ../../source/changes.txt $BuildDir/usr/share/doc/$Program/changelog
echo "creating installation..."
mkdir -p $BuildDir/usr/share/pixmaps/
mkdir -p $BuildDir/usr/share/applications
mkdir -p $BuildDir/usr/bin/
mkdir -p $BuildDir/usr/share/$Program
mkdir -p $BuildDir/usr/share/$Program/languages
mkdir -p $BuildDir/usr/share/$Program/help
mkdir -p $BuildDir/etc/udev/rules.d
install -m 644 general/icon.png $BuildDir/usr/share/pixmaps/$Program.png
install -m 644 general/$Program.desktop $BuildDir/usr/share/applications/$Program.desktop
sh copy_to_builddir.sh $Arch $BuildDir/usr/share/$Program
strip --strip-all $BuildDir/usr/share/$Program/avrusblaboszi
cp general/$Program.starter $BuildDir/usr/share/$Program/
chmod 755 $BuildDir/usr/share/$Program/$Program.starter
#cp general/46-usbavrlab.rules $BuildDir/etc/udev/rules.d
ln -s /usr/share/$Program/$Program.starter $BuildDir/usr/bin/$Program
#cp -r ../../help/* $BuildDir/usr/share/$Program/help
cp ../../languages/*.po $BuildDir/usr/share/$Program/languages
cp ../../languages/*.txt $BuildDir/usr/share/$Program/languages
echo "building package..."
dpkg-deb --build $BuildDir
cp $TmpDir/software_build.deb output/${Program}_${Version}_${Arch}-$Widgetset.deb
echo "cleaning up..."
#rm -r $BuildDir
