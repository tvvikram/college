<html>
	<head><title>Profile</title></head>
	<body style="margin:20">
	<?php
		$db_name="StudentAlumniDataBase"; // Database name
		$tbl_name="login"; // Table name
		$mysqlusername="mylcc";
		$mysqlpassword="mylcc";

		// Connect to server and select databse.
		$connection = mysql_connect("$host", "$mysqlusername", "$mysqlpassword")or die("cannot connect server... Authentication Failure");
		mysql_select_db("$db_name")or die("cannot select DB");

		@session_start();
		
		$id = $_SESSION['id'];
		
		$notes = "select * from notes where id = $id";
		
		$result = mysql_query($notes, $connection);
		$result = mysql_fetch_array($result);
	?>
		<br /><br /><br /><br /><br />
		<font size="4">Notes:</font><br /><br />
		<font size="4">Heading:</font>
		<?php echo $result[1] ?>
		<br /><br />
		<font size="4">Description:</font><br /><br />
		<?php echo $result[2] ?>
		<br /><br />
	</body>
	<br /><br /><br />
	<form method="post" action="notes.php">
		<input type="submit" value="Back"></input>	
	</form>
</html>