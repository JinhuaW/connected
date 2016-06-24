<?php   
    error_reporting(E_ALL);   
    set_time_limit(0);   
    echo "<h2>TCP/IP Connection</h2><br>";   
      
    $service_port = 7;   
    $address = "139.129.58.15";   
      
      
    $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);   
    if ($socket < 0)   
    {   
    	echo "socket_create() failed: reason: ".socket_strerror($socket)."<br>";   
    }   
    else  
    {   
    	echo "OK.<br>";   
    }   
      
    echo "试图连接 '$address' 端口 '$service_port'...<br>";   
    $result = socket_connect($socket, $address, $service_port);   
    if ($result < 0)   
    {   
   	 echo "socket_connect() failed.<br>Reason: ($result) ".socket_strerror($result)."<br>";   
    }   
    else  
    {   
   	 echo "连接OK<br>";   
    }   
      
    $in = date("Y-m-d H:i:s",time());   
    $out = '';   
      
    if (!socket_write($socket, $in, strlen($in)))   
    {   
    	echo "socket_write() failed: reason:".socket_strerror($socket)."<br>";   
    }   
    else  
    {   
    	echo "发送到服务器信息成功！<br>";   
    	echo "发送的内容为:<font color='red'>$in</font> <br>";   
    }   
      
    if ($out = socket_read($socket, 5555))   
    {   
    	echo "接收服务器回传信息成功！<br>";   
    	echo "接受的内容为:".$out."<br>";   
    }   
      
      
    echo "关闭SOCKET...<br>";   
    socket_close($socket);   
    echo "关闭OK<br>";   
?>  
