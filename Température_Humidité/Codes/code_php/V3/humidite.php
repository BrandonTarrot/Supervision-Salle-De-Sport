<?php
// --- CONFIGURATION BDD ---
$host = "localhost"; 
$user = "root";
$pass = ""; 
$db   = "supervision_salle_de_sport2";

$conn = new mysqli($host, $user, $pass, $db);
if ($conn->connect_error) { die("Erreur : " . $conn->connect_error); }

// Dernière mesure
$res_last = $conn->query("SELECT humidite FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 1");
$humi_actuelle = ($res_last && $row = $res_last->fetch_assoc()) ? $row['humidite'] : 0;

// Données graphique
$res_chart = $conn->query("SELECT humidite, date_mesure FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 20");
$labels = []; $data = [];
while($row = $res_chart->fetch_assoc()){
    $labels[] = date("H:i", strtotime($row['date_mesure']));
    $data[] = $row['humidite'];
}
$labels = array_reverse($labels);
$data = array_reverse($data);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Supervision Humidité</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.rawgit.com/Mikhus/canvas-gauges/master/gauge.min.js"></script>
    <meta http-equiv="refresh" content="10">
    <style>
        body { font-family: 'Segoe UI', sans-serif; background: #121212; color: #eee; margin: 0; padding: 20px; }
        .container { max-width: 1000px; margin: 0 auto; }
        
        /* --- BARRE DE NAVIGATION --- */
        .navbar {
            display: flex;
            justify-content: center;
            gap: 15px;
            margin-bottom: 30px;
            padding: 15px;
            background: #1e1e1e;
            border-radius: 12px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
        }
        .navbar a {
            color: white;
            text-decoration: none;
            padding: 10px 20px;
            border-radius: 8px;
            transition: 0.3s;
            font-weight: bold;
            font-size: 0.95rem;
            display: flex;
            align-items: center;
            gap: 8px;
        }
        .nav-home { border: 1px solid #555; }
        .nav-temp { background: #ff5722; } /* Orange */
        .nav-humi { background: #2ecc71; border: 1px solid white; } /* Vert (Actif) */
        .nav-hist { background: #444; border: 1px solid #2ecc71; }
        
        .navbar a:hover { 
            opacity: 0.9; 
            transform: translateY(-2px);
        }
        /* -------------------------- */

        .header { border-bottom: 2px solid #2ecc71; text-align: center; padding-bottom: 10px; margin-bottom: 20px; }
        .main-row { display: flex; gap: 20px; flex-wrap: wrap; margin-bottom: 20px; }
        .card { background: #1e1e1e; padding: 20px; border-radius: 12px; flex: 1; min-width: 300px; display: flex; flex-direction: column; align-items: center; box-shadow: 0 4px 15px rgba(0,0,0,0.3); }
        
        table { width: 100%; border-collapse: collapse; background: #1e1e1e; border-radius: 12px; overflow: hidden; margin-top: 15px; }
        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #333; }
        th { color: #2ecc71; background: #252525; text-transform: uppercase; font-size: 0.8rem; }
        
        .btn-back { display: inline-block; margin-top: 20px; padding: 10px 25px; border: 2px solid #2ecc71; color: #2ecc71; text-decoration: none; border-radius: 25px; transition: 0.3s; }
        .btn-back:hover { background: #2ecc71; color: #000; }
    </style>
</head>
<body>

    <div class="container">
        <!-- NOUVELLE BARRE DE NAVIGATION -->
        <div class="navbar">
            <a href="acceuil.html" class="nav-home">🏠 Accueil</a>
            <a href="temperature.php" class="nav-temp">🌡️ Température</a>
            <a href="humidite.php" class="nav-humi">💧 Humidité</a>
            <a href="historique.php" class="nav-hist">📜 Historique</a>
        </div>

        <div class="header"><h1>HUMIDITÉ - ZONE 1</h1></div>

        <div class="main-row">
            <div class="card">
                <h2>Actuel</h2>
                <canvas id="gaugeHumi"></canvas>
                <div style="font-size: 2.5em; font-weight: bold; color: #2ecc71;"><?php echo $humi_actuelle; ?>%</div>
            </div>
            <div class="card">
                <h2>Historique (Graph)</h2>
                <canvas id="chartHumi" style="width:100%; height:250px;"></canvas>
            </div>
        </div>

        <div class="card" style="align-items: stretch;">
            <h2>Derniers relevés</h2>
            <table>
                <thead><tr><th>ID</th><th>Valeur</th><th>Date</th></tr></thead>
                <tbody>
                    <?php 
                    $res_table = $conn->query("SELECT * FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 5");
                    if($res_table) {
                        while($row = $res_table->fetch_assoc()){
                            echo "<tr><td>#{$row['id']}</td><td><strong style='color:#2ecc71'>{$row['humidite']} %</strong></td><td>{$row['date_mesure']}</td></tr>";
                        }
                    }
                    ?>
                </tbody>
            </table>
            <div style="text-align: center;">
                <a href="acceuil.html" class="btn-back">RETOUR ACCUEIL</a>
            </div>
        </div>
    </div>

    <script>
        new RadialGauge({
            renderTo: 'gaugeHumi', width: 220, height: 220, minValue: 0, maxValue: 100,
            units: "%", majorTicks: ["0","20","40","60","80","100"], colorPlate: "transparent",
            colorNumbers: "#eee", borders: false, needleType: "arrow", animationDuration: 1500,
            value: <?php echo $humi_actuelle; ?>
        }).draw();

        new Chart(document.getElementById('chartHumi'), {
            type: 'line',
            data: {
                labels: <?php echo json_encode($labels); ?>,
                datasets: [{ 
                    label: 'Humidité (%)', 
                    data: <?php echo json_encode($data); ?>, 
                    borderColor: '#2ecc71', 
                    tension: 0.4, 
                    fill: true, 
                    backgroundColor: 'rgba(46,204,113,0.1)' 
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    y: { beginAtZero: true, max: 100 }
                }
            }
        });
    </script>
</body>
</html>