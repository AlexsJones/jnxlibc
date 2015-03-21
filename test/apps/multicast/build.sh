#!/usr/bin/env bash

if [ "$1" == "debug" ]
  make -f .Makefile debug
else
  make -f .Makefile
fi  
