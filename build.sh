#! /bin/bash
# compile and extract from image
rm -rf build/*
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/coord_single ./bin/coord_single
docker container cp extract:/code/build/src/bottom_left ./bin/bottom_left
docker container rm -f extract
