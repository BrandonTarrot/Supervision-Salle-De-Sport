<?php
require_once 'config.php';

// On récupère les dernières mesures de température et humidité
$sql = "SELECT id, temperature, humidite, date, zone FROM mesures_humidite_temperature ORDER BY date DESC";
$result = $conn->query($sql);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Supervision Salle de Sport</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; background-color: #f8f9fa; margin: 40px; }
        .container { max-width: 900px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h2 { color: #333; text-align: center; border-bottom: 3px solid #28a745; padding-bottom: 10px; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { padding: 12px; text-align: center; border-bottom: 1px solid #dee2e6; }
        th { background-color: #343a40; color: white; }
        .temp-val { color: #dc3545; font-weight: bold; }
        .hum-val { color: #007bff; font-weight: bold; }
        .badge-zone { background: #6c757d; color: white; padding: 3px 8px; border-radius: 4px; font-size: 0.9em; }
    </style>
</head>
<body>

<div class="container">
    <h2>Données de la Salle de Sport</h2>
    
    <table>
        <thead>
            <tr>
                <th>ID</th>
                <th>Zone</th>
                <th>Température</th>
                <th>Humidité</th>
                <th>Date / Heure</th>
            </tr>
        </thead>
        <tbody>
            <?php
            if ($result && $result->num_rows > 0) {
                while($row = $result->fetch_assoc()) {
                    echo "<tr>";
                    echo "<td>" . $row['id'] . "</td>";
                    echo "<td><span class='badge-zone'>Zone " . $row['zone'] . "</span></td>";
                    echo "<td class='temp-val'>" . $row['temperature'] . " °C</td>";
                    echo "<td class='hum-val'>" . $row['humidite'] . " %</td>";
                    echo "<td>" . $row['date'] . "</td>";
                    echo "</tr>";
                }
            } else {
                echo "<tr><td colspan='5'>Aucune donnée enregistrée pour le moment.</td></tr>";
            }
            ?>
        </tbody>
    </table>
</div>

</body>
</html>