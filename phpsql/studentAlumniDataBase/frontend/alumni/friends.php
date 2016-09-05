<html>
	<head><title>Friends</title></head>
	<body>
		<br />
		<div style="text-align: right">
		<form method="post" action="viewFriends.php">
			<input type="submit" value="View Your Friends"></input>
		</form>		
		</div>
		<br /><br /><br />
		You have a lot of friends, right? If not add them here...<br /><br />
		<form method="post" action="insertFriends.php">
			<input type="text" size="100" style="height: 30" name="friendsList"></input>
			<br /><br />
			Describe your friends:<br /><br />
			<input type="textarea" size="100" style="height: 100" name="description"></input><br /><br />
			<input type="submit" value="OK"></input>
		</form>
		<br /><br /><br />
	<form method="post" action="updateProfile.php">
		<input type="submit" value="Back"></input>	
	</form>
	</body>
</html>