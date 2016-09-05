<html>
	<head><title>New User</title></head>
	<body>
	<br /><br /><br /><br />
	<font size="5"><strong><h1><span style="color: blue">Stude</span><span style="color: red">nt Al</span><span style="color: brown">umni </span><span style="color: blue">Data</span><span style="color: green">Base</span></h1></strong></font>
	<center>	
	<?php
	ob_start();
	$host="localhost"; // Host name
	$mysqlusername=$_COOKIE['mysqlusername']; // Mysql username
	$mysqlpassword=$_COOKIE['mysqlpassword']; // Mysql password
	$db_name="StudentAlumniDataBase"; // Database name
	$mysqlusername="mylcc";
	$mysqlpassword="mylcc";
	
	$connection = mysql_connect("$host", "$mysqlusername", "$mysqlpassword")or die("cannot connect server... Authentication Failure");
	mysql_select_db($db_name) or die("Could not select database..." .mysql_error());

	@session_start();
	$id = $_SESSION['id'];
	
	$name = $_POST['name'];
	$address = $_POST['address'];
	$sex = $_POST['gender'];
	$email = $_POST['email'];
	
	$insert = "delete from person where id = $id";
	mysql_query($insert, $connection) or die("Could not execute query1" .mysql_error());
		
	$insert = "insert into person values ('$id', '$name', '$email', '$sex', '$address')";
	mysql_query($insert, $connection) or die("Could not execute query1" .mysql_error());
				
	$degreeLevel = $_POST['degreeLevel'];
	$degreeBranch = $_POST['degreeBranch'];
	$college = $_POST['college'];
	$graduationYear = $_POST['graduationYear'];
			
	$company = $_POST['company'];
	$companyAddress = $_POST['companyAddress'];
	$sector = $_POST['sector'];
	$companyEmail = $_POST['companyEmail'];
			
	$insert = "delete from degree where id = $id";
	mysql_query($insert, $connection) or die("Could not execute query2" .mysql_error());	
	
	$insert = "insert into degree values ($id, '$degreeLevel', '$degreeBranch', '$college', '$graduationYear')";
	mysql_query($insert, $connection) or die("Could not execute query2" .mysql_error());
	
	$insert = "delete from employment where id = $id";
	mysql_query($insert, $connection) or die("Could not execute query2" .mysql_error());	
			
	$insert = "insert into employment values ($id, '$company', '$companyAddress', '$sector', '$companyEmail')";
	mysql_query($insert, $connection) or die("Could not execute query3" .mysql_error());

	@session_start();
	$_SESSION['id'] = $id;	
	
	ob_end_flush();
	?>
	<br /><br /><br />
	<a href=profile.php>View Your Profile</a>
	</center>	
	</body>
</html>