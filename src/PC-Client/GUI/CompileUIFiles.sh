#!/bin/bash
#Find all .ui files in this directory and use 'uic' command to compiles them.
#Compiled header files will be put into the directory "$1".

cd $(dirname $0)
file_list=$(ls|grep .ui)
for file in $file_list
do
	header_file="UI_${file%.*}.h"
	echo "Compile $file to $1/${header_file}"
	uic $file -o $1/${header_file}
done

