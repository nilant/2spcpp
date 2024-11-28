#! /bin/bash
# compile and extract from image
rm -rf build/*
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/math4 ./bin/math4
docker container rm -f extract
