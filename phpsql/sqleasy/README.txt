
QUERY PROCESSOR:

Start querying the databases of MySql using browser.  Works under
`localhost' and MySql.

Uses HTML and PHP codes and provides a basic security.


SQLEASY:

Create the schema in a file. The program will detect and automatically
generate the required number of tuples!

Uses 'lex' file to pattern match the data-types and builds a matrix. The
matrix is used to generate tuples.



Requiements:
1. LAMP (Linux, Apache2, MySql, PHP) Server
2. Browser (Developed on Mozilla Firefox)

Commands:
a)
1. Use `make' to build the executive file.
2. Run the `sqleasy' file with file containing schema (path required) and number
	of tuples to generate as arguments.
3. Invoke `mysql' and read the schema file and the 'insert.db' file.
4. Use `make clean' to clean the folder.
5. Copy the QueryProcessor file to /var/www/ .
5. Connect to `localhost' and start querying...

b) Using shell script - build.sh
	Invoke build.sh with the schema containing file (path required) and the 
	 number of tuples to generate. Follow the shell script and start querying...	
	
	Requires /var/www/ folder with 777 permission.

Thanks for reading till here... :)
