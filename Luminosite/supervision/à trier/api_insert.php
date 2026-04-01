<?php
require_once 'config.php';

if(isset($_GET['lux']) && isset($_GET['zone'])) {
    $lux = $_GET['lux'];
    $zone = $_GET['zone'];
    $date = date("Y-m-d H:i:s");

    $sql = "INSERT INTO mesures_luminosite (luminosite, date, zone) VALUES ('$lux', '$date', '$zone')";

    if ($conn->query($sql) === TRUE) {
        echo "Succès";
    } else {
        echo "Erreur SQL";
    }
} else {
    echo "Données manquantes";
}
?>