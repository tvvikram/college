create table student(
	regNo char(10) primary key,
	name varchar(30) not null,
	major char(10),
	birthDate date
) ENGINE = INNODB;

create table course(
	courseNo int primary key,
	cname varchar(20) not null,
	dept varchar(20)
) ENGINE = INNODB;

create table enroll(
	regNo char(10),
	courseNo int,
	sem int,
	marks int,
	primary key(regNo, courseNo, sem),
	foreign key(regNo) references student(regNo),
	foreign key(courseNo) references course(courseNo)
) ENGINE = INNODB;

create table bookAdoption(
	courseNo int,
	sem int,
	bookISBN int,
	primary key(courseNo, sem),
	foreign key(bookISBN) references text(bookISBN)
) ENGINE = INNODB;

create table text(
	bookISBN int primary key,
	bookTitle varchar(30) not null,
	publisher varchar(30),
	author varchar(30)
) ENGINE = INNODB;




