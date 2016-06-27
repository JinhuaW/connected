<?php
$login_name = $_POST["login_name"];
$target_name = $_POST["target_name"];
$message = $_POST["message"];
if (empty($login_name) || empty($target_name) || empty($message)) {
	echo "login name, target name or message can't be empty!<br/>";
} else {
	$result = exec("./exchange $login_name $target_name \"$message\"", $output, $var);
	if ($var == 0) {
		//echo "Recv: $output[0]<br/>";
		foreach ($output as $file) {
			echo "$file<br/>";
		}
	} else {
		echo "Recv Error!<br>";
	}
}

/*
foreach ($output as $file) { 
	echo $file." ";
}
*/
/*
var_dump($output);
print_r($output);
*/
?>
