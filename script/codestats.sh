#1/bin/bash

result=`find . -regex ".*\.\(cpp\|h\|lua\)"`


filtered=`echo "$result" | grep -v extern`

num_files=`echo "$filtered" | wc -l`
echo "There are $num_files files"

num_lines=`echo "$filtered" | xargs wc -l | grep total`
echo
echo "There are $num_lines lines of code"

groups=`echo "$filtered" | sed "s?.*/\([^/]*/[^/]*\)?\1?" | sed "s?\([^/]*\).*?\1?" | sort -u`

for group in $groups; do
	echo "$group:"
	echo "$filtered" | grep $group | xargs wc -l | grep total
done

todos=`echo "$filtered" | xargs grep -i todo | wc -l`
echo
echo "$todos todos found"

for group in $groups; do
	echo "$group:"
	echo "$filtered" | grep $group | xargs grep -i todo | wc -l 
done

