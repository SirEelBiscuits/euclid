#!/bin/bash

result=`find . -regex ".*\.\(cpp\|h\|lua\)"`
filtered=`echo "$result" | grep -v extern`
echo "$filtered" | xargs grep -in todo
