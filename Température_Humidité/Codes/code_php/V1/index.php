<?php
include 'db_config.php';
$result = $conn->query("SELECT * FROM mesures_esp32 ORDER BY date_enregistrement DESC LIMIT 10");
?>

<!DOCTYPE html>
<html>
<head>
    <title>Supervision Salle de Sport</title>
    <meta charset="UTF-8">
    <style>
        body { font-family: Arial; text-align: center; background: #f4f4f4; }
        table { margin: 20px auto; border-collapse: collapse; background: white; }
        th, td { padding: 10px; border: 1px solid #ddd; }
        th { background: #007bff; color: white; }
    </style>
</head>
<body>
    <h1>Données de la Salle de Sport</h1>
    <table>
        <tr>
            <th>ID</th>
            <th>Température</th>
            <th>Humidité</th>
            <th>Date</th>
        </tr>
        <?php while($row = $result->fetch_assoc()): ?>
        <tr>
            <td><?php echo $row['id']; ?></td>
            <td><?php echo $row['temperature']; ?>°C</td>
            <td><?php echo $row['humidite']; ?>%</td>
            <td><?php echo $row['date_enregistrement']; ?></td>
        </tr>
        <?php endwhile; ?>
    </table>
</body>
</html>