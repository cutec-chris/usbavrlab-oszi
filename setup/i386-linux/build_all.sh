#!/bin/bash
mkdir output
#sh build_all_executables.sh gtk
#sh build_rpm.sh gtk
#sh build_deb.sh gtk
sh build_all_executables.sh gtk2
sh build_deb.sh gtk2
sh build_rpm.sh
mv /tmp/build/*.rpm output
