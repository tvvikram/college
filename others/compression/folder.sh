#!/bin/bash
make
folder=`zenity --file-selection --directory --title="Select Source Directory"`
dest=`zenity --file-selection --directory --title="Select Destination Directory"` 
if [ -z $dest ]
then
	zenity --error --title="Destination" --text="Not Found"
	exit 1
fi
files=`ls $folder/* `
if [ ! -e fldrcomp ]
then
	mkdir fldrcomp
fi
for file in $files
do
	if [ -f $file ]
	then
		comp=`echo "$file".mvh`
		result=`./mvh -c "$file" "$comp"`
		if [ $? -eq 0 ]
		then
			zenity --info --title="$file" --text="$result"
		fi
		mv "$comp"* "$dest/"

	fi
done
make clean
