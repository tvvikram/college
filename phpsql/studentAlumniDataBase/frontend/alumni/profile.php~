<html>
	<head><title>Profile</title></head>
	<body>
	<?php
		$db_name="StudentAlumniDataBase"; // Database name
		$tbl_name="login"; // Table name
		$mysqlusername="mylcc";
		$mysqlpassword="mylcc";

		// Connect to server and select databse.
		$connection = mysql_connect("$host", "$mysqlusername", "$mysqlpassword")or die("cannot connect server... Authentication Failure");
		mysql_select_db("$db_name")or die("cannot select DB");

		@session_start();
		
		$username = $_SESSION['username'];
		$password = $_SESSION['password'];

		$getUsername = "select * from login where username='$username' and userpassword='$password'";
		$resultUser = mysql_query($getUsername, $connection) or die("Could not execute query1" .mysql_error());
		$resultUser = mysql_fetch_array($resultUser) or die("Could not fetch array1" .mysql_error());
		$id = $resultUser[0];
		
		$name = "select * from person where id = $id";
		$resultName = mysql_query($name, $connection) or die("Could not execute query2" .mysql_error());
		$resultName = mysql_fetch_array($resultName) or die("Could not fetch array2" .mysql_error());
		
		$degree = "select * from degree where id = $id";
		$resultDegree = mysql_query($degree, $connection) or die("Could not execute query3" .mysql_error());
		$resultDegree = mysql_fetch_array($resultDegree) or die("Could not fetch array3" .mysql_error());
		
		$emp = "select * from employment where id = $id";
		$resultEmp = mysql_query($emp, $connection) or die("Could not execute query4" .mysql_error());
		$resultEmp = mysql_fetch_array($resultEmp) or die("Could not fetch array4" .mysql_error());
		?>
		<br />
		<center><font size="5"><strong>Your Profile</strong></font></center><br /><br />
		<hr color="blue" width="90%" size="3">
		<font size="5">
		<pre>
	<strong>Personal Details:</strong>
		
		Name		: <?php echo $resultName[1];?>
		
		Address		: <?php echo $resultName[4];?>
		
		Gender		: <?php echo $resultName[3];?>
		
		Email		: <?php echo $resultName[2];?>
		
		<hr color="blue" width="90%" size="3">	
	<strong>Qualifications:</strong>
		
	  <strong>Degree details:</strong>
		
		Degree		: <?php echo $resultDegree[1];?>
		
		Branch/Subject	: <?php echo $resultDegree[2];?>
		
		College		: <?php echo $resultDegree[3];?>
		
		Graduation Year	: <?php echo $resultDegree[4];?>
		
		
		
	  <strong>Professional Details:</strong>
		
		Company		: <?php echo $resultEmp[1];?>
		
		Company Address	: <?php echo $resultEmp[2];?>
		
		Sector		: <?php echo $resultEmp[3];?>
		
		Company Email	: <?php echo $resultEmp[4];?>
		
		
		
		<hr color="blue" width="90%" size="3">
		
		Your Unique ID in Student Alumni DataBase is <?php echo $id;?>.
		
		<hr color="blue" width="90%" size="3"></pre>
		</font><center><font size="1">User Profile</font></center><br />
	</body>

</html>