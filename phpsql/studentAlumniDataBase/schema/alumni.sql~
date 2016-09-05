drop table person;
drop table degree;
drop table employment;
drop table friends;
drop table activities;
drop table notes;
drop table login;

create table login
(
	id int(5) NOT NULL auto_increment,
	username varchar(30) NOT NULL,
	userpassword varchar(70) NOT NULL,
	PRIMARY KEY(id)
) ENGINE=INNODB;

create table person 
(
	id int(5) NOT NULL,
	name varchar(30) NOT NULL,
	email varchar(30),
	gender varchar(6),
	address varchar(50),
	FOREIGN KEY(id) REFERENCES login(id)
) ENGINE=INNODB;

create table degree 
(
	id int(5) NOT NULL,
	degreeLevel varchar(25),
	degreeBranch varchar(50),
	college varchar(50),
	graduationYear int(4),
	FOREIGN KEY(id) REFERENCES login(id)
) ENGINE=INNODB;

create table employment 
(
	id int(5) NOT NULL,
	company varchar(30),
	companyAddress varchar(100),
	sector varchar(30),
	companyEmail varchar(30),
	FOREIGN KEY(id) REFERENCES login(id)
) ENGINE=INNODB;

create table activities 
(
	id int(5) NOT NULL,
	activityList varchar(50),
	description varchar(200),
	FOREIGN KEY(id) REFERENCES login(id)
) ENGINE=INNODB;

create table friends
(
	id int(5) NOT NULL,
	friendsList varchar(100),
	description varchar(500),
	FOREIGN KEY(id) REFERENCES login(id)
) ENGINE=INNODB;

create table notes
(
	id int(5) NOT NULL,
	heading varchar(20),
	description varchar(200),
	FOREIGN KEY(id) REFERENCES login(id)
) ENGINE=INNODB;

alter table login auto_increment = 2859;
