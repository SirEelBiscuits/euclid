#!/bin/bash
./msbuild.sh Shipping
cp -r content/* package
cp -r engine/bin/Shipping/* package
