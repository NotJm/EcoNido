<?php
require_once("config/dbconnection.php");

// Assuming $_GET['deviceId'] is sent with the AJAX request
$deviceId = $_GET['deviceName'];

$db = new DataBaseConnection();
$connection = $db->GetConnection();
$query = "SELECT * FROM deviceState WHERE deviceName = '$deviceName'";
$response = $connection->query($query);
$result = ($response->num_rows > 0) ? $response->fetch_assoc() : null;

echo json_encode($result);

