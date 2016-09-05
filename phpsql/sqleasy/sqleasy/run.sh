#!/bin/bash
# to `make` the random tuple generation file

make

# test if `make` was successful
if test $? -eq 0; then
	if test $# -ne 2; then
		echo -e "Rebuilding input data...\n";
		echo -e "Enter filename:\c";
		read filename;
		echo -e "Enter number of tuples to generate:\c";
		read n;
		
	else
		filename=$1;
		n=$2;
	fi
	
	./sqleasy $filename $n
	mv insert.db ../files/insert.db
	
	
else
	echo -e "Make not successful. I am quitting...\n";
fi

make clean
