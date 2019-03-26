#!/bin/sh

rm -rf pdf; mkdir pdf; cd pdf

for MODE in "pPb_MU" "pPb_MD" "Pbp_MU" "Pbp_MD"; do

  root -l -q -b ../scripts/drawSame.C"(\"../output\",\"${MODE}\")"

done
