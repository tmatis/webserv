<?php

	if (isset($_COOKIE["user"]))
	{
		echo 'hello ';
		echo $_COOKIE["user"];
	}
	else
	{
		if (isset($_GET["username"]))
		{
			setcookie("user", $_GET["username"], time() + 60 * 10,'/');
			echo 'hello ';
			echo $_GET["username"];
		}
		else
		{
			print "<h1>Who are you ?</h1>";
			print "<form action=\"cookie.php\" method=\"GET\">";
			print "username: <input type=\"text\" name=\"username\"><br>";
			print "<input type=\"submit\" value=\"Send\"></form>";
		}
	}
?>