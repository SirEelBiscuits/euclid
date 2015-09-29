#!/bin/bash

result=`find . -regex ".*\.\(cpp\|h\|lua\)"`
filtered=`echo "$result" | grep -v extern`
makefiles=`find . | grep makefile | grep -v extern`
echo "$filtered $makefiles" | xargs grep -in todo

