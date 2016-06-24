<?php
$login_name = $_POST["login_name"];
$target_name = $_POST["target_name"];
$message = $_POST["message"];
$result = exec("./exchange $login_name $target_name \"$message\"", $output, $var);
/*
foreach ($output as $file) { 
	echo $file." ";
}
*/
echo "$output[0]<br/>";
/*
var_dump($output);
print_r($output);
*/
?>
<table border=1>
<?php
for($i=0;$i<4;$i++)
{
?>
<tr>
<td>
11111111111111111
</td>
<td>
11111111111111111
</td>
</tr>
<?php
}
?>
</table>
