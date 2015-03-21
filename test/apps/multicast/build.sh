#!/usr/bin/env bash

if [ "$1" == "debug" ]
  make -f Make.file debug
else
  make -f Make.file
fi  
