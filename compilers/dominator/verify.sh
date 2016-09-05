#!/bin/bash

#script to run dominators a large number of times with different input
#size and make sure the dominator trees match in each run.

repeat=100
max_input_size=600

j=0
while [ $j -lt $repeat ]
do
	i=3
	while [ $i -le $max_input_size ]
	do
	
		if ! ./dominators $i > /dev/null 2> /dev/null
		then
			echo "Verification failed..."
			exit 1
		fi
		i=$(($i+1))
	done
	j=$(($j+1))
	echo "$j Successfull passes"
done
	
exit 0
