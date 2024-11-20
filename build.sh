#! /bin/bash
# compile and extract from image
rm -rf build/*
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/math1 ./bin/math1
docker container rm -f extract
