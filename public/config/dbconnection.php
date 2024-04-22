<?php
class DataBaseConnection
{
    // Creacion de un atributo para el manejo de la base de datos
    private $connection;

    // Definicion del constructor de la clase
    public function __construct()
    {
        // Requerir los datos o credenciales de conexion a la base de datos
        require_once("config/config.php");

        // Creacion la instancia de la conexion a base de datos
        $this->connection = new mysqli(
            DATABASE_HOST,
            DATABASE_USER,
            DATABASE_PASSWORD,
            DATABASE_NAME
        );

        // Manejo de errores
        if($this->connection->connect_error)
        {
            
            die(
                'Error: Al conectar con la base de datos causa:' +
                $this->connection->connect_error
            );
            
        }
    }

    // Creamos un metodo para obtener la conexion
    public function GetConnection() { return $this->connection; }

    // Creamos metodo para cerrar la conexion con las base de datos
    public function CloseConnection() { $this->connection->close(); }

}
