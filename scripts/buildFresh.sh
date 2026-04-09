#!/bin/bash

$REPO_ROOT = $(git rev-parse --show-toplevel)

# move out of scripts/
cd "$REPO_ROOT" 

sudo rm -rf build/

mkdir build

cd build

cmake ..

make

cd ../scripts

