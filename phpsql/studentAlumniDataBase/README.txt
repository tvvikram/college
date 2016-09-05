DBMS Laboratory Project on
	Student Alumni DataBase

Commands:
a. Login to root account and execute the following:
$mysql -u root -p

create user 'mylcc'@'localhost' identified by 'mylcc'

create database StudentAlumniDataBase;

grant all on StudentAlumniDataBase.* to 'mylcc'@'localhost';

quit

b. cd to dbmslab/schema

c. Execute the following in mylcc user account:
$mysql -u mylcc -p
password is mylcc

use StudentAlumniDataBase
source alumni.sql

d. copy the frontend/alumni to /www/var/

Thanks...
