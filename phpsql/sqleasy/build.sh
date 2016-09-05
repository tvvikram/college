#!/bin/bash
# a simple front end for configuring the Query Processor

echo -e "You have to create a database in MySql and load the required schema to MySql...\n";
echo -e "Do you wish to create database and load the schema?[yn]:\c";
read answer;
if test $answer = "y" || $answer = "Y"; then
	echo -e "Enter database to be created:\c";
	read db;
	echo -e "Enter schema containing file:\c";
	read schema;
	echo -e "Entering MySql as root...\n";
	mysql -u root -p --execute="create database $db; use $db; source $schema; quit;"
else
	echo -e "No attempt to create database and load the schema on user request...\n";
fi

echo -e "Do you wish to continue generating random tuples for a schema and insert them?[yn]:\c";
read answer;
if test $answer = "y" || $answer = "Y"; then
	echo -e "Resolving dependencies...\n";
	make
	if test $? -eq 0; then
		if test $# -ne 2; then
			echo -e "Enter schema containing file:\c";
			read filename;
			echo -e "Enter number of tuples to generate:\c";
			read n;
		else
			filename=$1;
			n=$2;
		fi

		echo -e "Generating random tuples...\n";
		./files/sqleasy $filename $n
		echo -e "Moving the insert.db file to ./files\n";
		mv insert.db ./files/

		echo -e "\nEnter the database to be used in MySql:\c";
		read dbname;

		echo -e "\nEntering MySql as root...";
		mysql -u root -p --execute="use $dbname; source ./files/insert.db;"
	
		echo -e "Query Processor configured successfully.\n";
	else
		echo -e "Make not successful. I am quitting...\n";
	fi

	make clean
else
	echo -e "No tuples generated...\n";
fi

echo -e "Do you want to connect to localhost?[yn] \c";
read answer;
if test $answer = "y" || $answer = "Y"; then
	echo -e "Creating 'QueryProcessor' directory under /var/www/ \n";
	mkdir /var/www/QueryProcessor
	echo -e "Copying contents of QueryProcessor to /var/www/QueryProcessor/ \n";
	cp ./QueryProcessor/* /var/www/QueryProcessor
	echo -e "Starting localhost... \n";
	firefox http://localhost/QueryProcessor/getPassword.html
fi

echo -e "Thank You for using Query Processor";
echo -e "Cheers:-)VIKRAM\n";
