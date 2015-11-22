#!/bin/bash
#Find all .ui files in this directory and use 'uic' command to compiles them.
#Compiled header files will be put into the directory "$1".

cd $(dirname $0)
file_list=$(ls|grep .qrc)
for file in $file_list
do
	header_file="RES_${file%.*}.h"
	echo "Compile $file to $1/${header_file}"
	rcc $file --name ${file%.*} -o $1/${header_file}
done

