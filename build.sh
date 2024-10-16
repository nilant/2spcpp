#! /bin/bash
# compile and extract from image
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/matheuristic ./bin/matheuristic
docker container rm -f extract

# prepare archive
zip -r deploy.zip bin
