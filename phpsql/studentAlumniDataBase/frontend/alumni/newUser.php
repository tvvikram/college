<html>
	<head><title>New User</title></head>
	<body>
	<pre>
	<font size="5"><strong><h1><span style="color: blue">Stude</span><span style="color: red">nt Al</span><span style="color: brown">umni </span><span style="color: blue">Data</span><span style="color: green">Base</span></h1></strong></font>
	<font size="5"><strong>Create an Account:</strong></font><br></br>
		<font size="4">Please fill this form to get a new account.</font><br></br>
		<hr color="green" width="90%" size="3">
			<form method="post" action="insertTuples.php">
			<font size="4"><strong>Personal Details:</strong></font><br></br>
			Name		: <input type="text" size="30" style="height: 30" name="name" />
			
			Address		: <input type="textarea" size="30" style="height: 60" name="address" />

			Sex		: <input type="radio" name="gender" value="male"/> Male <input type="radio" name="gender" value="Female"/> Female

			Email		: <input type="text" size="30" style="height: 30" name="email" />
			
			<hr color="green" width="90%" size="3">
			
			<font size="4"><strong>Qualifications:</strong></font><br></br>
			Degree		: <select name='degreeLevel'>
				<option value="UG">UG</option>
				<option value="PG">PG</option>
				<option value="PhD">PhD</option>
			</select>
			
			Branch/Subject	: <input type="text" size="30" style="height: 30" name="degreeBranch" />
			
			College		: <input type="text" size="30" style="height: 30" name="college" />
			
			Graduation Year	: <select name='graduationYear'>
			<?php
				for ($i = date("Y") + 5; $i >= 1970; $i = $i - 1)	{	?>
					<option value="<?php echo $i ?>"><?php echo $i ?></option>
		<?php	}	?>					
			</select>
			
			


			
			<font size="4">Give some of your professional details:</font>
			
			
			Company		: <input type="text" size="51" style="height: 30" name="company" />
			
			Company address	: <input type="textarea" size="30" style="height: 60" name="companyAddress" />	
			
			Company is in <select name="sector">
															<option value="information">Information</option>
															<option value="business">Business</option>
															<option value="agriculture">Agriculture</option>
															<option value="others">Other</option>
														</select> sector.
			
			Email		: <input type="text" size="30" style="height: 30" name="companyEmail" />
														
			
			<hr color="green" width="90%" size="3">
			
			<font size="4"><strong>Your Login Details:</strong></font>
			
			Select a Username: <input type="text" size="30" style="height: 30" name="username" />

			Select a Password: <input type="password" size="30" style="height: 30" name="password1" />

			Re-enter Password: <input type="password" size="30" style="height: 30" name="password2" />
					
			<input type="submit" height="30" value="Submit" />
			
			<hr color="green" width="90%" size="3">
			<center><font size="1">Registration for new user</font></center></form></pre>		
	</body></html>