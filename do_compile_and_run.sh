#!/bin/bash

mpic++.openmpi main.cpp
if [ $? -ne 0 ]
then
  exit 1
fi


mpirun.openmpi -n 200 ./a.out
