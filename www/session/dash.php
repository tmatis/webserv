<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bulma@0.9.3/css/bulma.min.css">
	<title>DASH</title>
</head>
<body>
	<h1 class="title">Session demo dash</h1>
	<?php
		session_start();
		if(!isset($_SESSION['login']))
		{
			header("Location: login.php");
		}
		else
		{
			echo "<h1>Welcome ".$_SESSION['login']."</h1>";
			echo "<h1>your mail is ".$_SESSION['mail']."</h1>";


		}
	?>
	<button class="button is-primary" onclick="location.href='logout.php'">Logout</button>
</body>
</html>
