<?php
	$mysqluser = $_POST['mysqluser'];
	$mysqlpassword = $_POST['mysqlpassword'];
	setcookie("mysqlusername", $mysqluser, false, "/", false);
	setcookie("mysqlpassword", $mysqlpassword, false, "/", false);
?>




<html>
	<head><title>Login</title></head>
	<body><br />
		<center><font size="5"><strong><h1><span style="color: blue">Stude</span><span style="color: red">nt Al</span><span style="color: brown">umni </span><span style="color: blue">Data</span><span style="color: green">Base</span></h1></strong></font></center>
		<br /><br />		
		<table width="300" border="0" align="center" cellpadding="0" cellspacing="1" bgcolor="#CCCCCC">
		<tr>
		<form name="form1" method="post" action="checkLogin.php">
		<td>
		<table width="100%" border="0" cellpadding="3" cellspacing="1" bgcolor="#FFFFFF">
		<tr>
		<td colspan="3"><strong>Member Login </strong></td>
		</tr>
		<tr>
		<td width="78">Username</td>
		<td width="6">:</td>
		<td width="294"><input name="username" type="text" id="username"></td>
		</tr>
		<tr>
		<td>Password</td>
		<td>:</td>
		<td><input name="password" type="password" id="password"></td>
		</tr>
		<tr>
		<td>&nbsp;</td>
		<td>&nbsp;</td>
		<td><input type="submit" name="Submit" value="Login"></td>
		</tr>
		</table>
		</td>
		</form>
		</tr>
		</table>
		<br /><br />
		<center>New User? <a href=newUser.php>Register here.</a></center>
	</body>
</html>