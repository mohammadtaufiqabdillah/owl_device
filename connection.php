<?php
if (session_status() == PHP_SESSION_NONE) {
    session_start();
}

$serverName = 'localhost';
$usernameDB = 'root';
$passwordDB = 'kepo1';
$databaseName = 'owl_device';

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

$conn = new mysqli($serverName, $usernameDB, $passwordDB, $databaseName);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>