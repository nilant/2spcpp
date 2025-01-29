#! /bin/bash
# compile and extract from image
rm -rf build/*
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/lp1_plus ./bin/lp1_plus
docker container cp extract:/code/build/src/lp2_plus ./bin/lp2_plus
docker container rm -f extract
