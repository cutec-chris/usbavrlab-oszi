#!/bin/bash
Widgetset=$1
if [ "x$Widgetset" = "x" ]; then
  Widgetset=gtk2
fi
echo "compiling for $1..."
mkdir ../../output
mkdir ../../output/i386-linux
rm ../../output/i386-linux/avrusblaboszi
lazbuild -B --widgetset=$1 ../../source/avrusblaboszi.lpi
