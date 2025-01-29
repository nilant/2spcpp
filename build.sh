#! /bin/bash
# compile and extract from image
rm -rf build/*
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/lp1 ./bin/lp1
docker container cp extract:/code/build/src/lp2 ./bin/lp2
docker container rm -f extract
