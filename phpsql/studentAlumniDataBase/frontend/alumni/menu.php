<html>
<head>
<title>HTML Frames Example - Menu 1</title>
<style type="text/css">
body {
	font-family:verdana,arial,sans-serif;
	font-size:15pt;
	margin:10px;
	background-color:#ff9980;
	}
</style>
</head>
	<body background="back.gif">
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
				
		@session_start();
		$id = $_SESSION['id'];
	
		$user = "select * from person where id = $id";
		$user = mysql_query($user, $connection) or die("Could not execute query" .mysql_error());
		$user = mysql_fetch_array($user) or die("Could not fetch array" .mysql_error());
		$user = $user[1];
	?>
		<h4><span style="color: blue">Hello<br /></span> <span style="color: green"><?php echo $user ?></span></h4>
		<a href="welcome.html" target="right">Home</a><br /><br />
		<a href="profile.php" target="right">Profile</a><br /><br />
		<a href="updateProfile.php" target="right">Update profile</a><br /><br />
		<a href="updateUser.php" target="right">Update account</a><br /><br />
		<a href="logout.php" target="_top">Logout</a><br />	<br />
	</body>
	
</html>