<?php
// No log
$infile = fopen("test.text", "r") or die("Unable to open file!");
echo fread($infile,filesize("test.text"));
fclose($infile);

// Log
sqreenOn();
$infile = fopen("test.text", "r") or die("Unable to open file!");
echo fread($infile,filesize("test.text"));
fclose($infile);

// No log
sqreenOff();
$infile = fopen("test.text", "r") or die("Unable to open file!");
echo fread($infile,filesize("test.text"));
fclose($infile);


?>