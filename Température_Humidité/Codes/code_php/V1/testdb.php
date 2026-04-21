<?php
$conn = new mysqli("localhost","root","","supervision_salle_de_sport");

if($conn->connect_error){
 die("Connexion échouée");
}

echo "Connexion réussie";
?>