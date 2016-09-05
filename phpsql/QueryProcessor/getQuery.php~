<?php
	$user = $_POST['user'];
	$password = $_POST['password'];
	setcookie("username", $user, false, "/", false);
	setcookie("password", $password, false, "/", false);
?>
<html>
	<head><title>Query</title></head>
	<body bgcolor="white">
	<br /><br /><br />
	<center>
		<font size="5"><strong><h1><span style="color: blue">Que</span><span style="color: red">ry </span><span style="color: brown">Pro</span><span style="color: blue">ces</span><span style="color: green">sor</span></h1></strong><br /></font>
		<font size="3"><form method="post" action="processQuery.php">
			<input type="text" size="80" style="height: 30" class="lst" name="query" /><br /><br />
			<?php
				@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
				$query = "show databases";
				$result = mysql_query($query, $connection);
				$num_of_rows = mysql_num_rows($result);
			?>
				<input type="submit" style="height: 30" value="Process Query" />
				<input type="submit" style="height: 30" value="I'm Feeling Lucky" /><br />
				
				Database:
				
			<?php
				for ($i = 0; $i < $num_of_rows; $i = $i + 1)
				{
					$row = mysql_fetch_array($result); ?>
					<input type="radio" value="<?php echo $row[0] ?>" name="DB"> <?php echo "$row[0]" ?> <tr />
			<?php
				}
			?>
			
			
			<br /><br /><a href="databases.php">Click here to access all the databases</a><br />
			
				
		</form></font>
	<br /><br /><br /><br /><br /><br /><br /><br />
	<font size="2">Query Processing by <a href=mailto:tvvikram.sjce@gmail.com>VIKRAM TV</a>.<br /> Suggestions are welcome.</font></center>
	</body>
</html>