#!/bin/bash

# Programs
centroid=../build/tests/centroid
nearest=../build/tests/nearest

# Parameters
n=1000    # Samples
data="neches brazos cinaruco"

for i in $data 
do
  $centroid -samples $n < $i.txt > $i-centroid.txt
  $nearest -samples $n < $i.txt > $i-nearest.txt
done
