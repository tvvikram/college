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
		
		$friends = "select * from friends where id = $id";
		
		$result = mysql_query($friends, $connection);
		$result = mysql_fetch_array($result);
	?>
		<br /><br /><br /><br /><br />
		<font size="4">Your friends are:</font><br /><br />
		<?php echo $result[1] ?>
		<br /><br />
		<font size="4">You described your friends as:</font><br /><br />
		<?php echo $result[2] ?>
		<br /><br />
	</body>
	<br /><br /><br />
	<form method="post" action="friends.php">
		<input type="submit" value="Back"></input>	
	</form>
</html>