<?php require_once('config.php'); ?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Historique Complet</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>

<div class="container">
    <a href="dashboard_lux.php" style="text-decoration: none; color: #3498db;">← Retour au Dashboard</a>
    <h1>📊 Historique Complet</h1>

    <table>
        <thead>
            <tr>
                <th>Date</th>
                <th>Valeur Brute</th>
                <th>Intensité</th>
            </tr>
        </thead>
        <tbody>
            <?php 
                foreach (array_reverse($historique) as $mesure) {
                // On vérifie si les clés existent avant de les afficher
                    $timestamp = $mesure['timestamp'] ?? 'N/A';
                    $raw_value = $mesure['raw_value'] ?? 'Non dispo'; // Si raw_value n'existe pas, on affiche 'Non dispo'
                    $lux = $mesure['luminosite'] ?? 0;

                    echo "<tr>";
                    echo "<td>" . $timestamp . "</td>";
                    echo "<td>" . $raw_value . "</td>";
                    echo "<td>" . number_format($lux, 1) . " lx</td>";
                    echo "</tr>";
                }
            ?>
        </tbody>
    </table>
</div>

</body>
</html>