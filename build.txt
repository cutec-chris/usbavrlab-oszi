#!/bin/bash
cd $(dirname "$0")
git submodule sync --recursive
git submodule update --init --recursive
lazbuild source/usbavrlaboszi.lpi
