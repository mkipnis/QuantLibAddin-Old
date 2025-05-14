#!/bin/bash

docker build --no-cache -t ghcr.io/mkipnis/quantlib-addin-old:latest -f docker_build_all .
#docker push ghcr.io/mkipnis/quantlib-addin-old:latest 
