<html>
	<head><title>Databases</title></head>
	<body><center>
	<font size="5"><strong><h1><span style="color: blue">Que</span><span style="color: red">ry </span><span style="color: brown">Pro</span><span style="color: blue">ces</span><span style="color: green">sor</span></h1></strong><br /></font>
<?php
	$user = $_COOKIE['username'];
	$password = $_COOKIE['password'];
	echo "User : $user";
?>
		
<?php
	@$connection = mysql_connect("localhost", "$user", "$password") or die("Could not connect to server...  " .mysql_error());
	$query = "show databases";
	$result = mysql_query($query, $connection);
	$num_of_rows = mysql_num_rows($result);
	$num_of_fields = mysql_num_fields($result);
?>

<?php
	echo "<br />Number of Databases:  <strong>$num_of_rows.</strong><br /><br />";
?>
	<table cellSpacing="2" cellPadding="6" align="center" border="1">
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
				echo "<tr>";?>
				<form method="POST" action="tables.php">
				<?php
				for ($i = 0; $i < $num_of_fields; $i = $i + 1)
				{
					echo "<td>";?>
					<input type="submit" value="<?php echo $row[0] ?>" name="db"><tr />
					<?php echo "</td>";
				}
				echo "</tr>";
			}
				?>
			</form>			
		</table>
	<br /><br /><br /><br />
	<font size="2">Query Processing by <a href=mailto:tvvikram.sjce@gmail.com>VIKRAM TV</a>.<br /> Suggestions are welcome.</font></center></center>
</body>
</html>