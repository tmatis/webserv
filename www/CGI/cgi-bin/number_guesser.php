<!DOCTYPE html>

<html>
    <head>
        <meta charset="UTF-8">
        <title>Number Guesser</title>
    </head>

	<style>
		body
		{
			background-color: rgb(25, 42, 86);
		}

		h1
		{
			margin-top: 20%;
			text-align: center;
			font-size: 4em;
			font-family: monospace;
			outline: none;
			color: rgb(5, 13, 32);	
		}

		h3
		{
			text-align: center;
			font-size: 3em;
			font-family: monospace;
			outline: none;
			color: rgb(5, 13, 32);	
		}
	</style>

    <body>
<?php

if ($_SERVER["REQUEST_METHOD"] == "GET" || $_SERVER["REQUEST_METHOD"] == "POST")
{
	if ($_SERVER["REQUEST_METHOD"] == "GET")
	{
		if (isset($_GET["num"]) && is_numeric($_GET["num"]))
		{
			$guess = $_GET["num"];
		}
		else
		{
			print "<h1>Invalid value !</h1>";
			exit("BAD_VALUE");
		}
	}
	else
	{
		if (isset($_POST["num"]) && is_numeric($_POST["num"]))
		{
			$guess = $_POST["num"];
		}
		else
		{
			print "<h1>Invalid value !</h1>";
			exit("BAD_VALUE");
		}
	}

	$number	= rand(1, 5);
	if ($guess == $number)
	{
		print "<h1>Good job, you guessed right</h1>";
		print "<h3><a href=\"/php.html\"> let's find out if you were just lucky ! </a></h3>";
	}
	else
	{
		print "<h1>You lose, i thought of <strong>$number</strong></h1>";
		print "<h3><a href=\"/php.html\"> try again ! </a></h3>";
	}
}
else
{
	print "<h1>Invalid REQUEST_METHOD !</h1>";
}

?>
    </body>
</html>