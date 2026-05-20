<?php
// --- CONFIGURATION CONNEXION RASPBERRY ---
$host = "172.19.6.120"; 
$user = "api";
$pass = "api"; 
$db   = "supervision_salle_de_sport2";

$conn = new mysqli($host, $user, $pass, $db);

if ($conn->connect_error) {
    die("Erreur de connexion");
}

// 1. Récupération de la dernière valeur
$res_last = $conn->query("SELECT humidite, zone FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 1");
$humi_actuelle = 0;
$zone_actuelle = "N/A";

if ($res_last && $row = $res_last->fetch_assoc()) {
    $humi_actuelle = $row['humidite'];
    $zone_actuelle = $row['zone'];
}

// 2. Récupération des 10 dernières données pour le graphique
$res_chart = $conn->query("SELECT humidite, date FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 10");
$labels = []; $data = [];
if ($res_chart) {
    while($row = $res_chart->fetch_assoc()){
        $labels[] = date("H:i", strtotime($row['date']));
        $data[] = $row['humidite'];
    }
}
$labels = array_reverse($labels);
$data = array_reverse($data);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Humidité - Style Moderne</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.rawgit.com/Mikhus/canvas-gauges/master/gauge.min.js"></script>
    <meta http-equiv="refresh" content="60">
    <style>
        :root { --accent: #2ecc71; --bg: #0d0d0d; --card: #161616; }
        body { background-color: var(--bg); color: #eee; font-family: 'Segoe UI', sans-serif; margin: 0; padding: 20px; }
        .container { max-width: 1100px; margin: auto; }
        .navbar { display: flex; justify-content: center; gap: 12px; background: #1a1a1a; padding: 15px; border-radius: 10px; margin-bottom: 30px; border: 1px solid #222; }
        .navbar a { color: white; text-decoration: none; padding: 10px 22px; border-radius: 6px; font-weight: bold; background: #333; }
        .nav-humi { background: var(--accent) !important; color: #000 !important; }
        .grid { display: grid; grid-template-columns: 400px 1fr; gap: 25px; }
        .card { background: var(--card); padding: 25px; border-radius: 15px; border: 1px solid #222; text-align: center; }
        h1 { color: var(--accent); text-align: center; text-transform: uppercase; }
        .valeur-digitale { font-size: 3.5rem; font-weight: bold; color: var(--accent); margin-top: -45px; }
    </style>
</head>
<body>

<div class="container">
    <div class="navbar">
        <a href="accueil.php"> Accueil</a>
        <a href="temperature.php" style="background:#e67e22"> Température</a>
        <a href="humidite.php" class="nav-humi"> Humidité</a>
        <a href="historique.php"> Historique</a>
    </div>

    <h1>SUPERVISION HUMIDITÉ</h1>
    <div style="text-align:center; color:#555; margin-bottom:20px;">Dernière donnée : Zone <?php echo $zone_actuelle; ?></div>

    <div class="grid">
        <div class="card">
            <h3>Direct (0-100%)</h3>
            <canvas id="gaugeHumi"></canvas>
            <div class="valeur-digitale"><?php echo round($humi_actuelle); ?>%</div>
        </div>
        <div class="card">
            <h3>Historique Récent</h3>
            <div style="height: 320px;"><canvas id="chartHumi"></canvas></div>
        </div>
    </div>
</div>

<script>
    // Initialisation de la jauge avec la valeur PHP injectée
    var gauge = new RadialGauge({
        renderTo: 'gaugeHumi',
        width: 380,
        height: 380,
        units: "%",
        minValue: 0,
        maxValue: 100,
        value: <?php echo $humi_actuelle; ?>, // FORCE LA VALEUR ICI
        startAngle: 90,
        ticksAngle: 180,
        strokeTicks: false,
        colorPlate: "transparent",
        colorMajorTicks: "#444",
        colorNumbers: "#888",
        highlights: [{ "from": 0, "to": 100, "color": "#2ecc71" }],
        needleType: "arrow",
        needleWidth: 4,
        colorNeedle: "#fff",
        valueBox: false,
        borders: false,
        animationDuration: 1500,
        animationRule: "decelerate"
    }).draw();

    // Graphique Chart.js
    new Chart(document.getElementById('chartHumi'), {
        type: 'line',
        data: {
            labels: <?php echo json_encode($labels); ?>,
            datasets: [{
                label: 'Humidité (%)',
                data: <?php echo json_encode($data); ?>,
                borderColor: '#2ecc71',
                fill: true,
                backgroundColor: 'rgba(46, 204, 113, 0.1)',
                tension: 0.3
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: { y: { beginAtZero: true, max: 100 } }
        }
    });
</script>
</body>
</html>