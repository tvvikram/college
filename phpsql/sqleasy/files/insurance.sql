create table person
(
	driverId char(10),
	name varchar(20),
	address varchar(30),
	primary key(driverId)
) ENGINE = INNODB;

create table car
(
	regNo char(10),
	model char(20),
	year date,
	primary key(regNo)
) ENGINE = INNODB;

create table accident
(
	reportNumber int,
	accidentDate date,
	location varchar(30),
	primary key(reportNumber, accidentDate)
) ENGINE = INNODB;

create table owns
(
	driverId char(10),
	regNo char(10),
	primary key(driverId, regNo),
	foreign key(driverId) references person(driverId),
	foreign key(regNo) references car(regNo)
) ENGINE = INNODB;

create table participated
(
	driverId char(10),
	regNo char(10),
	reportNumber int,
	damageAmount int,
	primary key(driverId, regNo, reportNumber),
	foreign key(driverId) references person(driverId),
	foreign key(regNo) references car(regNo),
	foreign key(reportNumber) references accident(reportNumber)
) ENGINE = INNODB;



