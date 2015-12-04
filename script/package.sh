#!/bin/bash
./msbuild.sh rebuild Shipping
rm -r package/*
cp -r content/* package
cp -r engine/bin/Shipping/* package
