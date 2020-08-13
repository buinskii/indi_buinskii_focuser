#!/usr/bin/env bash

cd ./build

cmake .. --debug-output
make
sudo make install

cd -