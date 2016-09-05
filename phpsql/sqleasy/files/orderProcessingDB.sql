create table customer
(
	custId int,
	cname varchar(30),
	city varchar(20),
	primary key (custId)
) ENGINE = INNODB;

create table orderProcessing
(
	orderNo int primary key,
	odate date,
	custId int,
	ordAmt int,
	foreign key(custId) references customer(custId)
) ENGINE = INNODB;

create table orderItem
(
	orderNo int,
	itemNo int,
	quantity int,
	foreign key(orderNo) references orderProcessing(orderNo),
	foreign key(itemNo) references item(itemNo)
) ENGINE = INNODB;

create table item
(
	itemNo int primary key,
	unitPrice int
) ENGINE = INNODB;

create table shipment
(
	orderNo int,
	warehouseNo int,
	shipDate date,
	foreign key(orderNo) references orderProcessing(orderNo),
	foreign key(warehouseNo) references warehouse(warehouseNo)
) ENGINE = INNODB;

create table warehouse
(
	warehouseNo int primary key,
	city varchar(20)
) ENGINE = INNODB;



