<html>
	<head><title>Notes</title></head>
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

	$heading = $_POST['heading'];
	$description = $_POST['description'];
		
	@session_start();
	$id = $_SESSION['id'];

	$insert = "delete from notes where id = $id";
	mysql_query($insert, $connection) or die("could not execute query" .mysql_error());		
	
	$insert = "insert into notes values ($id, '$heading', '$description')";
	mysql_query($insert, $connection) or die("could not execute query" .mysql_error());
	?>
	Notes Written.
	<table>
	<tr>
	<td>
		<form method="post" action="notes.php">
			<input type="submit" value="Write Again"></input>
		</form>
	</td><td></td>
	<td>
		<form method="post" action="viewNotes.php">
			<input type="submit" value="View Archives"></input>		
		</form>	
	</td>
	</tr>	
	</table>
	</body>
</html>