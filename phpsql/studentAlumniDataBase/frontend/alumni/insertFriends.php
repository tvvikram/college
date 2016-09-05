<html>
	<head><title>Activities</title></head>
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

	$friendsList = $_POST['friendsList'];
	$description = $_POST['description'];
		
	@session_start();
	$id = $_SESSION['id'];
	
	$insert = "delete from friends where id = $id";
	mysql_query($insert, $connection) or die("could not execute query" .mysql_error());		
	
	$insert = "insert into friends values ($id, '$friendsList', '$description')";
	mysql_query($insert, $connection) or die("could not execute query" .mysql_error());
	?>
	Friends Updated.
	<table>
	<tr>
	<td>
		<form method="post" action="friends.php">
			<input type="submit" value="Add Friends"></input>
		</form>
	</td><td></td>
	<td>
		<form method="post" action="viewFriends.php">
			<input type="submit" value="View Friends"></input>		
		</form>	
	</td>
	</tr>	
	</table>
	</body>
</html>