<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bulma@0.9.3/css/bulma.min.css">
	<title>Document</title>
</head>
<body>
<h1 class="title">Session demo</h1>


<form action="login.php" method="post">
<span class="tag is-dark">login</span>

	<input type="text" class="input is-primary" name="login" placeholder="Login">
	<span class="tag is-dark">email</span>

	<input type="text" class="input is-primary" name="mail" placeholder="mail">
	<input class="button" type="submit" value="Go">
<?php

session_start();

if ($_SERVER["REQUEST_METHOD"] == "POST") {
	// do logic
	$_SESSION['login'] = $_POST['login'];
	$_SESSION['mail'] = $_POST['mail'];
}

// check if user is logged in
if (isset($_SESSION['login']))
{
	header('Location: dash.php');
}

?>

</body>
</html>


