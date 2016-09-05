#!/bin/bash
echo -e "This application will generate a random 3 address
IR (Intermediate Representation) and converts it into
SSA (Static - Single Assignment) Form";
rm *.o
make
if test $# -eq 1; then
	n=$1
else
	echo -e "Enter the number of statements (per nesting) you need:\c";
	read n
fi
echo -e "SSA will now run with argument $n.\n";
./ssa.out $n
#dotty generatedIR.dot
#dotty SSA.dot
./folderClean.sh
