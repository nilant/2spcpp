#! /bin/bash
# compile and extract from image
colima start
mkdir -p bin
docker build --target builder -t builder .
docker container create --name extract builder
docker container cp extract:/code/build/src/shelves ./bin/shelves
docker container cp extract:/code/build/src/coord ./bin/coord
docker container cp extract:/code/build/src/coord2 ./bin/coord2
docker container cp extract:/code/build/src/bottom_left ./bin/bottom_left
docker container rm -f extract
colima stop
# prepare archive
zip -r deploy.zip bin
