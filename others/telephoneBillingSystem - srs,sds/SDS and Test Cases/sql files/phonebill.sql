drop table if exists payment;
drop table if exists phoneNumber;
drop table if exists callLog;
drop table if exists meterReading;
drop table if exists bill;
drop table if exists  address;
drop table if exists tariff;
drop table if exists login;
drop table if exists customer;


create table customer
(
	customerId int AUTO_INCREMENT,
	firstName varchar(30) not null,
	middleName varchar(30),
	lastName varchar(30) not null,
	primary key (customerId)
) ENGINE = INNODB;

create table address
(
	customerId int,
	dno varchar(6)	,
	street1 varchar(30),
	street2 varchar(30),
	city varchar(30) not null,
	district varchar(30),
	state varchar(	30),
	pincode int(6) not null,
	primary key (customerId),
	foreign key (customerId) references customer(customerId)
) ENGINE = INNODB;

create table login
(
	username varchar(10),
	userpassword char(64),
	customerId int,
	primary key (customerId),
	foreign key (customerId) references customer(customerId)
) ENGINE = INNODB;

create table tariff
(
	tariffPlan int,
	monthlyCharges numeric(10,2),
	freeCalls int,
	local numeric(4,2),
	std numeric(4,2),
	isd numeric(4,2),
	tax numeric(5,2),
	primary key (tariffPlan)
) ENGINE = INNODB;

create table phoneNumber
(
	customerId int,
	phoneNumber int,
	tariffPlan int, 
	connection date,
	deposit numeric(10,2),
	primary key(customerId, phoneNumber),
	foreign key(customerId) references customer(customerId),
	foreign key(tariffPlan) references tariff(tariffPlan)
) ENGINE = INNODB;

create table callLog
(
	customerId int,
	status char,	-- r - received, d - dialled
	phoneNumber int,
	startTime int(10),
	endTime int(10),
	primary key(phoneNumber, startTime),
	foreign key (customerId) references customer(customerId)
) ENGINE = INNODB;

create table meterReading
(
	customerId int,
	month int(6),
	startMeter int,
	endMeter int,
	primary key(customerId,month),
	foreign key (customerId) references customer(customerId)
) ENGINE = INNODB;

create table bill
(
	billNo int(20),
	customerId int,	
	month int(6)	,
	callCharges numeric(10,2) default NULL,
	debit numeric(10,2) default NULL,
	tax numeric(8,2) default NULL,
	credit numeric(10,2) default NULL,
	net numeric(10,2) default NULL,
	misc numeric(5,2) default NULL,
	primary key (customerId, month),
	unique (billNo),
	foreign key (customerId) references customer(customerId)
)  ENGINE = INNODB;

create table payment
(
	paymentNo int(20),
	billNo int(20),
	payDate date,
	billAmount numeric(10,2),
	paidAmount numeric(10,2),
	primary key (paymentNo),
	foreign key (billNo) references bill(billNo)
) ENGINE = INNODB;

