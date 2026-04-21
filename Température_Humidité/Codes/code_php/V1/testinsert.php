<?php

$conn = new mysqli("localhost","root","","supervision_salle_de_sport");

if($conn->connect_error){
 die("Erreur connexion : " . $conn->connect_error);
}

$sql = "INSERT INTO mesures_esp32 (temperature, humidite) VALUES (25,60)";

if($conn->query($sql) === TRUE){
 echo "INSERTION OK";
}else{
 echo "ERREUR SQL : " . $conn->error;
}

$conn->close();

?>