<?php
// Configuration
$url = "http://172.19.6.120:5000/api/luminosite/historique";
$options = array('http' => array('timeout' => 5));
$context = stream_context_create($options);

// Récupération
$data = @file_get_contents($url, false, $context);

// Analyse
if ($data === FALSE) {
    $historique = [];
    $erreur_message = "Erreur : Impossible de joindre le serveur Python.";
} else {
    $historique = json_decode($data, true);
    $erreur_message = null;
}
?>