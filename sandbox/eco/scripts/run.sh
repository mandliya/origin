#!/bin/bash

# Parameters
data="neches brazos cinaruco tambopata"
n=100

# Programs and options
samples="-samples $n"
weight="-weight weight.txt"
centroid="../build/tests/centroid $samples"
nearest="../build/tests/nearest $samples"
wcentroid="../build/tests/centroid $samples $weight"
wnearest="../build/tests/nearest $samples $weight"

for i in $data 
do
  $centroid < $i.txt > $i-cd.csv
  $nearest < $i.txt > $i-nnd.csv
  $wcentroid < $i.txt > $i-wcd.csv
  $wnearest < $i.txt > $i-wnnd.csv
done
