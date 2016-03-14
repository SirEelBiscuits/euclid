#!/bin/bash
./msbuild.sh rebuild Shipping

mkdir -p package

rm -rf package/*
cp -r content/* package
cp -r engine/bin/Shipping/* package
