<?php
if ($_SERVER['REQUEST_METHOD'] == "POST") {
    // Obtención de la conexión
    require_once("config/dbconnection.php");

    // Llamamos y creamos instancia de la clase
    $connection = new DataBaseConnection();
    $cursor = $connection->GetConnection();

    // Obtenemos datos
    $device = isset($_POST['device']) ? $_POST['device'] : null;
    $temperature = isset($_POST['temperature']) ? $_POST['temperature'] : null;
    $humidity = isset($_POST['humidity']) ? $_POST['humidity'] : null;
    $mq = isset($_POST['mq']) ? $_POST['mq'] : null;
    $quality = isset($_POST['calidad']) ? $_POST['calidad'] : null;

    // Confirmamos que exista en la base de datos
    $exists = "SELECT * FROM deviceState WHERE deviceName='$device'";
    $response = $cursor->query($exists);

    if ($response->num_rows > 0) {
        // Actualizamos para la tabla de deviceState
        $update = "UPDATE deviceState SET temperature='$temperature', humidity='$humidity', mq='$mq', quality='$quality' WHERE deviceName='$device'";
        $cursor->query($update);

        echo "*** DATOS ACTUALIZADOS *** <BR>";
        echo "{DEVICE: '$device', DIS= '$temperature', LIM='$humidity'}";
    } else {
        // La tarjeta no existe, la agregamos
        $insertNewDevice = "INSERT INTO deviceState(deviceName, temperature, humidity, mq, quality) VALUES ('$device', '$temperature', '$humidity', '$mq', '$quality')";
        $cursor->query($insertNewDevice);

        echo "*** NUEVO DISPOSITIVO AGREGADO *** <BR>";
        echo "{DEVICE: '$device', DIS= '$temperature', LIM='$humidity'}";
    }
}
?>
