<?php
	$user = $_POST['user'];
	$password = $_POST['password'];
	setcookie("username", $user, false, "/", false);
	setcookie("password", $password, false, "/", false);
?>
<html>
	<head><title>Query</title></head>
	<body bgcolor="white">
	<br /><br />
	<center>
		<font size="5"><strong><h1><span style="color: blue">Stude</span><span style="color: red">nt Al</span><span style="color: brown">umni </span><span style="color: blue">Data</span><span style="color: green">Base</span></h1></strong><br /></font>
		<font size="3"><form method="post" action="processQuery.php">
			<input type="text" size="70" style="height: 32" class="lst" name="query" /><br /><br />
			<?php
				@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
				$db = 'lccsjce';
				mysql_select_db($db, $connection) or die("Could not open database!!" .mysql_error());				
				$query = "show tables";
				$result = mysql_query($query, $connection);
				$num_of_rows = mysql_num_rows($result);
			?>
				<input type="submit" style="height: 30" value="Process Query" />
				<input type="submit" style="height: 30" value="I'm Feeling Lucky" /><br />
				
				Tables:
				
			<?php
				for ($i = 0; $i < $num_of_rows; $i = $i + 1)
				{
					$row = mysql_fetch_array($result); ?>
					<input type="radio" value="<?php echo $row[0] ?>" name="DB"> <?php echo "$row[0]" ?> <tr />
			<?php
				}
			?>
			
				
		</form></font>
	<br /><br /><br /><br /><br /><br /><br /><br />
	</body>
</html>