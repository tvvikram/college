
<html>
	<head><title><?php echo "Tuples in $tuples of Database $db" ?></title></head>
	<body><center>
		

<?php
	$user = $_COOKIE['username'];
	$password = $_COOKIE['password'];
	$db = $_COOKIE['db'];
	$tuples = $_POST['tuples'];
?>	
<font size="5"><strong><h1><span style="color: blue">Que</span><span style="color: red">ry </span><span style="color: brown">Pro</span><span style="color: blue">ces</span><span style="color: green">sor</span></h1></strong><br /></font>
<?php
	@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
	mysql_select_db($db, $connection) or die("Could not open $db.");				
	$query = "select * from $tuples";
	$result = mysql_query($query, $connection) or die("Could not execute query: $query.  Probably no such database!!");
	$num_of_rows = mysql_num_rows($result);
	$num_of_fields = mysql_num_fields($result);

	echo "User : $user<br />";
	echo "Database : $db<br />";
	echo "Tuple : $tuples<br />";
		
	echo "<br />Number of rows:  <strong>$num_of_rows.</strong><br />";
	echo "Number of fields: <strong>$num_of_fields.</strong><br /><br />";
?>
<table cellSpacing="2" cellPadding="6" align="center" border="1">
	<tr>
		<td colspan="7">
  			<h3 align="center"> <?php echo $db ?> </h3>
		</td>
	</tr>
<?php
	for ($i = 0; $i < $num_of_fields; $i = $i + 1)
	{
		$head = mysql_field_name($result, $i);
		echo "<td>";
		echo "<strong>$head</strong>";
		echo "</td>";
	}
	while ($row = mysql_fetch_array($result))
	{				
		echo "<tr>";
		for ($i = 0; $i < $num_of_fields; $i = $i + 1)
		{
			echo "<td>";
			echo $row[$i];
			echo "</td>";
		}
			echo "</tr>";
	}
	?>
</table>
<br /><br /><br /><br />
<font size="2">Query Processing by <a href=mailto:tvvikram.sjce@gmail.com>VIKRAM TV</a>.<br /> Suggestions are welcome.</font></center></center></center>
</body>
</html>