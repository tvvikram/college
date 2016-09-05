<html>
	<head><title>New User</title></head>
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
	
	$username = $_POST["username"];
	$checkUser = "select * from login where login.username='$username'";
	$result = mysql_query($checkUser, $connection) or die("Could not execute query..." .mysql_error());
	$num_of_rows = mysql_num_rows($result);
	
	if ($num_of_rows == 1)
	{
		echo "Username already exists!!";
		echo "<br /><br /><br /><br />";
	?>
	<a href=newUser.php>Click here to go back</a>
	<?php
	}
	else
	{
		$password1 = $_POST['password1'];
		$password2 = $_POST['password2'];
		if ($password1 != $password2)
		{
			echo "Passwords do not match!!";
			echo "<br /><br /><br /><br />";
		?>
		<a href=newUser.php>Click here to go back</a>
		<?php
		}
		else
		{
			$password1 = md5($password1);
			$insertToLogin = "insert into login(username, userpassword) values('$username', '$password1')";
			mysql_query($insertToLogin, $connection) or die("Could not insert username and password to login table" .mysql_error());
			$idQuery = "select id from login where username='$username'";
			$id = mysql_query($idQuery, $connection);
			$id = mysql_fetch_array($id);
			$id = $id[0];
			echo "Your Username : $username";
			echo "<br />";
			echo "Your Password : $password2";
			echo "<br />";
			echo "Your Unique ID:$id";
			echo "<br />";
		
			
			@session_start();
			$_SESSION['username'] = $username;
			$_SESSION['password'] = $password2;
			$_SESSION['id'] = $id;
		
			$name = $_POST['name'];
			$address = $_POST['address'];
			$sex = $_POST['gender'];
			$email = $_POST['email'];
			
			$insert = "insert into person values ('$id', '$name', '$email', '$sex', '$address')";
			mysql_query($insert, $connection) or die("Could not execute query" .mysql_error());?>
			<br /><br /><br />
			<center><a href=panel.html>Move to your page.</a></center><?php
			
			$degreeLevel = $_POST['degreeLevel'];
			$degreeBranch = $_POST['degreeBranch'];
			$college = $_POST['college'];
			$graduationYear = $_POST['graduationYear'];
			
			$company = $_POST['company'];
			$companyAddress = $_POST['companyAddress'];
			$sector = $_POST['sector'];
			$companyEmail = $_POST['companyEmail'];
			
			$insert = "insert into degree values ($id, '$degreeLevel', '$degreeBranch', '$college', '$graduationYear')";
			mysql_query($insert, $connection) or die("Could not execute query" .mysql_error());
			
			$insert = "insert into employment values ($id, '$company', '$companyAddress', '$sector', '$companyEmail')";
			mysql_query($insert, $connection) or die("Could not execute query" .mysql_error());
		}
	}
	ob_end_flush();
	?>
	</center>	
	</body>
</html>
