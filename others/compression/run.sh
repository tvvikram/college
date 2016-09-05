#!/bin/bash
make
if [ ! -x mvh ]
then
	zenity --error --title="Fatal Error" --text="Unable to Install Software!"
	exit -1
fi
choice=` zenity --list --radiolist --column="Select" --column="Hidden" --column="Option"  0 0 Compress 1 1 "Decompress" --hide-column=2`
if [ $? -eq 0 ]
then
	if [ $choice -eq 0 ]
	then
		src=`zenity --file-selection --title="Compress" --text="Select Source File"`
		if [ -z $src ];then zenity --error --title="Compress" --text="Source File Not Selected" ; exit 1;fi
		tgt=`zenity --file-selection --title="Compress" --save --file-filter="Minimal Variance Huffman | *.mvh"  --confirm-overwrite --text="Select Destination File"`
		if [ -z $tgt ];then zenity --error  --title="Compress"  --text="Destination File Not Selected" ; exit 2;fi
		tgt=`echo "$tgt".mvh`
		zenity --question --title='Dotty' --text="Request Huffman Tree?"
		if [ $? -eq 0 ];then
			result=`./mvh -c --dotty "$src" "$tgt"`
			nohup dotty dotty.dot & 
			zenity --info  --text="$result"
		else 
			result=`./mvh -c "$src" "$tgt"`
			zenity --info --text="$result"
		fi
	elif [ $choice -eq 1 ]
	then
		src=`zenity --file-selection --file-filter="Minimal Variance Huffman | *.mvh" --title="Decompress" --text="Select Source File"`
		if [ -z $src ];then zenity --error --title="Decompress" --text="Source File Not Selected" ; exit 11;fi
		dic=`echo "$src".dic`
		if [ ! -r $dic ];then zenity --error --title="Decompression" --text="Dictionary '$dic' Not Found";exit  12;fi
		tgt=`zenity --file-selection --title="Decompress" --save --confirm-overwrite --text="Select Destination File"`
		if [ -z $tgt ];then zenity --error --title="decompress" --text="Destination File Not Selected" ; exit 13;fi
		./mvh -d "$src" "$tgt"
	else
		zenity --error --title="Hello" --text="Did you select Anything?"
	fi
	make clean
	exit 0
fi
make clean
exit -1
