#!/bin/bash
rm -rf /tmp/www
cp -a www /tmp/www/
cd ./srcs/cmake-build-debug/ && ./srcs
