<html>
	<head><title>Tuples</title></head>
	<body bgcolor="white">
	<center>
<?php
	$user = $_COOKIE['username'];
	$password = $_COOKIE['password'];
	@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
	$db = $_POST['DB'];
?>

		<font size="2"><strong><h1><span style="color: blue">Que</span><span style="color: red">ry </span><span style="color: brown">Pro</span><span style="color: blue">ces</span><span style="color: green">sor</span></h1></strong><br /></font>
<form method="post" action="processQuery.php">
Enter Query: 
			<input type="text" size="80" style="height: 30" class="lst" name="query" />
			<?php
				@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
				$query = "show databases";
				$result = mysql_query($query, $connection);
				$num_of_rows = mysql_num_rows($result);
			?>
				<input type="submit" style="height: 30" value="Process Query" /><br />
				My database: <?php echo $_POST['DB'];?>
				Database:
				<?php
				for ($i = 0; $i < $num_of_rows; $i = $i + 1)
				{
					$row = mysql_fetch_array($result); ?>
					<input type="radio" value="<?php echo $row[0] ?>" name="DB"> <?php echo "$row[0]" ?> <tr />
			<?php
				}
			?>
</form>	

<?php	
	echo "<br /><br />You selected the database <strong>$db.</strong><br />";
	mysql_select_db($db, $connection) or die("Could not open $db.");

	$query = $_POST['query'];
	$query = stripcslashes($query);
	echo "You entered the query <strong>$query.</strong><br />";	
	
	$startTime = microtime();
	$result = mysql_query($query, $connection) or die("Could not execute query: $query.  Probably no such database!!");
	$endTime = microtime();	
	$runTime = $endTime - $startTime;
	$num_of_rows = mysql_num_rows($result);
	$num_of_fields = mysql_num_fields($result);
	echo "<br />Number of rows:  <strong>$num_of_rows.</strong><br />";
	echo "Number of fields: <strong>$num_of_fields.</strong><br /><br />";
	echo "Query ran for <strong>$runTime</strong> seconds.<br /><br /><br />";
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


<font size="2"><strong><h1><span style="color: blue">Que</span><span style="color: red">ry </span><span style="color: brown">Pro</span><span style="color: blue">ces</span><span style="color: green">sor</span></h1></strong></font>
<form method="post" action="processQuery.php">
Enter Query: 
			<input type="text" size="80" style="height: 30" class="lst" name="query" />
			<?php
				@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
				$query = "show databases";
				$result = mysql_query($query, $connection);
				$num_of_rows = mysql_num_rows($result);
			?>
				<input type="submit" style="height: 30" value="Process Query" /><br />
				Database:
				
			<?php
				for ($i = 0; $i < $num_of_rows; $i = $i + 1)
				{
					$row = mysql_fetch_array($result); ?>
					<input type="radio" value="<?php echo $row[0] ?>" name="DB"> <?php echo "$row[0]" ?> <tr />
			<?php
				}
			?>
</form>		
	
<br /><br /><br /><br />
	
	<font size="2">Query Processing by <a href=mailto:tvvikram.sjce@gmail.com>VIKRAM TV</a>.<br /> Suggestions are welcome.</font>
<br /><br /><br />	

	</center>
	</body>
</html>
