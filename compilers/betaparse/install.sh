#!/bin/sh
make clean >/dev/null 2>/dev/null
if [ ! -r /etc/shadow ]
then	
	sudo "$0"
	exit
fi
printf "Installing Betaparse......" 
i=0
while [ $i -lt 3 	]
do
	printf "\b\b\b |"
	sleep .1
	printf "\b\b /"
	sleep .1
	printf "\b\b --"
	sleep .1
	printf "\b\b\b \\ "
	sleep .1
	i=`expr $i + 1`
done
printf "\b\b\b   \n"
make >/dev/null 2>/dev/null
if [ $? -eq 0 ]
then 
	chmod 755 betaparse
	cp betaparse /bin/betaparse
	if [ $? -eq 0 ]
	then
		echo "Installation Success\n"
	else
		echo "Installation Failed\n"
	fi
else
	echo "Compilation Failed!!\n"	
	make clean >/dev/null
fi
make clean >/dev/null
