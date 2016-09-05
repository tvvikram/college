<?php
ob_start();
$host="localhost"; // Host name
$mysqlusername=$_COOKIE['mysqlusername']; // Mysql username
$mysqlpassword=$_COOKIE['mysqlpassword']; // Mysql password
$db_name="StudentAlumniDataBase"; // Database name
$tbl_name="login"; // Table name
$mysqlusername="mylcc";
$mysqlpassword="mylcc";


// Connect to server and select databse.
mysql_connect("$host", "$mysqlusername", "$mysqlpassword")or die("cannot connect server... Authentication Failure");
mysql_select_db("$db_name")or die("cannot select DB");

// username and password sent from form
$username=$_POST['username'];
$password=$_POST['password'];

$username = stripslashes($username);
$password = stripslashes($password);
$username = mysql_real_escape_string($username);
$password = mysql_real_escape_string($password);

$password = md5($password);
$sql="SELECT * FROM $tbl_name WHERE username='$username' and userpassword='$password'";
$result=mysql_query($sql) or die("Could not execute query" .mysql_error());

// Mysql_num_row is counting table row
$count=mysql_num_rows($result);
// If result matched $myusername and $mypassword, table row must be 1 row
	if($count==1){
		// Register $username, $password and redirect to file "loginSuccess.php"
		@session_start();
		$_SESSION['username'] = $username;
		$_SESSION['password'] = $password;
		$result = mysql_fetch_array($result);
		$_SESSION['id'] = $result[0];
		header("location:loginSuccess.php");
	}
	else {
		echo "<br /><br /><br />";
		echo "Wrong Username or Password";
		?><br /><br />
		Create an account?<a href=newUser.php>Click here</a><br /><br /><br />
		<?php
	}
ob_end_flush();
?>