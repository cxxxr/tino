#!/bin/sh

cd userland
./make.sh
cd ../

cd kernel
./make.sh
cd ../

cd loader
./run.sh
