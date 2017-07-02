#!/bin/bash
cd $(dirname "$0")
git submodule sync --recursive
git submodule update --init --recursive
lazbuild components/synapse/laz_synapse.lpk
lazbuild components/general/general.lpk
lazbuild source/avrusblaboszi.lpi
