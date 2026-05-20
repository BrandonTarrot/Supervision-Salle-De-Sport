<?php
// --- CONFIGURATION CONNEXION ---
// D'après ton erreur, vérifie si c'est 'supervision_salle_de_sport' ou 'supervision_salle_de_sport2'
$host = "localhost";
$user = "root";
$pass = "";
$db   = "supervision_salle_de_sport"; // Corrigé selon ta capture d'écran de code

$conn = new mysqli($host, $user, $pass, $db);

// Vérification robuste de la connexion
if ($conn->connect_error) {
    die("<div style='color:red; padding:20px; border:2px solid red;'>
            <strong>Erreur de connexion :</strong> " . $conn->connect_error . " <br>
            <em>Vérifiez que le nom de la base est bien '$db' dans PhpMyAdmin.</em>
         </div>");
}

// 1. Récupération de la dernière valeur (pour les jauges/afficheurs)
$query_last = "SELECT * FROM mesures_esp32 ORDER BY id DESC LIMIT 1";
$result_last = $conn->query($query_last);
$data_last = $result_last->fetch_assoc();

$temp_actuelle = $data_last['temperature'] ?? 0;
$humi_actuelle = $data_last['humidite'] ?? 0;

// 2. Récupération de l'historique (pour le graphique - 15 derniers points)
$query_chart = "SELECT temperature, humidite, date FROM mesures_esp32 ORDER BY id DESC LIMIT 15";
$result_chart = $conn->query($query_chart);

$labels = [];
$temps = [];
$humis = [];

while($row = $result_chart->fetch_assoc()){
    $labels[] = date("H:i", strtotime($row['date']));
    $temps[] = $row['temperature'];
    $humis[] = $row['humidite'];
}

// Inversion pour l'ordre chronologique (gauche à droite)
$labels = array_reverse($labels);
$temps = array_reverse($temps);
$humis = array_reverse($humis);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Supervision ESP32</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <meta http-equiv="refresh" content="5"> <!-- Rafraîchit toutes les 5 sec -->
    
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background-color: #f4f7f9;
            margin: 0;
            padding: 20px;
            color: #333;
        }
        .container {
            max-width: 1000px;
            margin: auto;
        }
        .header {
            text-align: center;
            padding: 20px;
            background: #fff;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            margin-bottom: 20px;
        }
        .grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-bottom: 20px;
        }
        .card {
            background: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            text-align: center;
        }
        .valeur {
            font-size: 3rem;
            font-weight: bold;
            margin: 10px 0;
        }
        .temp { color: #ff5722; }
        .humi { color: #2196f3; }
        
        .chart-box {
            background: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            margin-top: 20px;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
            background: #fff;
        }
        th, td {
            padding: 12px;
            border-bottom: 1px solid #ddd;
            text-align: center;
        }
        th { background-color: #eee; }
        
        .alert {
            padding: 10px;
            background: #ffebee;
            color: #c62828;
            border-radius: 5px;
            margin-top: 10px;
            display: <?php echo ($temp_actuelle > 30) ? 'block' : 'none'; ?>;
        }
    </style>
</head>
<body>

<div class="container">
    <div class="header">
        <h1>Système de Mesure Température & Hygrométrie</h1>
        <p>Statut : <span style="color: green;">● Connecté (ESP32)</span></p>
    </div>

    <div class="grid">
        <!-- Carte Température -->
        <div class="card">
            <h3>Température</h3>
            <div class="valeur temp"><?php echo $temp_actuelle; ?>°C</div>
            <input type="range" min="0" max="50" value="<?php echo $temp_actuelle; ?>" disabled style="width:80%">
            <div class="alert">⚠️ Seuil dépassé : Ventilateur activé !</div>
        </div>

        <!-- Carte Humidité -->
        <div class="card">
            <h3>Humidité</h3>
            <div class="valeur humi"><?php echo $humi_actuelle; ?>%</div>
            <input type="range" min="0" max="100" value="<?php echo $humi_actuelle; ?>" disabled style="width:80%">
        </div>
    </div>

    <!-- Graphique -->
    <div class="chart-box">
        <h3>Historique en temps réel</h3>
        <canvas id="myChart" height="100"></canvas>
    </div>

    <!-- Tableau -->
    <table>
        <thead>
            <tr>
                <th>ID</th>
                <th>Température</th>
                <th>Humidité</th>
                <th>Date / Heure</th>
            </tr>
        </thead>
        <tbody>
            <?php
            $result_table = $conn->query("SELECT * FROM mesures_esp32 ORDER BY id DESC LIMIT 5");
            while($row = $result_table->fetch_assoc()){
                echo "<tr>
                        <td>{$row['id']}</td>
                        <td>{$row['temperature']}°C</td>
                        <td>{$row['humidite']}%</td>
                        <td>{$row['date']}</td>
                      </tr>";
            }
            ?>
        </tbody>
    </table>
</div>

<script>
const ctx = document.getElementById('myChart').getContext('2d');
new Chart(ctx, {
    type: 'line',
    data: {
        labels: <?php echo json_encode($labels); ?>,
        datasets: [{
            label: 'Température (°C)',
            data: <?php echo json_encode($temps); ?>,
            borderColor: '#ff5722',
            tension: 0.3,
            fill: false
        }, {
            label: 'Humidité (%)',
            data: <?php echo json_encode($humis); ?>,
            borderColor: '#2196f3',
            tension: 0.3,
            fill: false
        }]
    },
    options: {
        responsive: true,
        scales: { y: { beginAtZero: false } }
    }
});
</script>

</body>
</html>