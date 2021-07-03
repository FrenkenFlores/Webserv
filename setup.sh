#!/bin/bash
rm -rf /tmp/www
#rm -rf srcs/build
cp -a www /tmp/www/
cmake -S srcs/ -B srcs/build
# cmake --build srcs/build

