<?php
// --- CONFIGURATION CONNEXION RASPBERRY ---
$host = "172.19.6.120"; 
$user = "api";
$pass = "api"; 
$db   = "supervision_salle_de_sport2";

$conn = new mysqli($host, $user, $pass, $db);

if ($conn->connect_error) {
    die("<div style='color:red; text-align:center;'>Erreur de connexion au Raspberry Pi</div>");
}

// 1. Récupération de la dernière valeur réelle
$res_last = $conn->query("SELECT temperature, zone FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 1");
$temp_actuelle = 0;
$zone_actuelle = "N/A";

if ($res_last && $row = $res_last->fetch_assoc()) {
    $temp_actuelle = $row['temperature'];
    $zone_actuelle = $row['zone'];
}

// 2. Données pour le graphique (10 derniers points)
$res_chart = $conn->query("SELECT temperature, date FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 10");
$labels = []; $data = [];
if ($res_chart) {
    while($row = $res_chart->fetch_assoc()){
        $labels[] = date("H:i", strtotime($row['date']));
        $data[] = $row['temperature'];
    }
}
$labels = array_reverse($labels);
$data = array_reverse($data);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Température - Supervision</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.rawgit.com/Mikhus/canvas-gauges/master/gauge.min.js"></script>
    
    <!-- Actualisation automatique toutes les 60 secondes -->
    <meta http-equiv="refresh" content="60">

    <style>
        :root { --accent: #e67e22; --bg: #0d0d0d; --card: #161616; }
        body { background-color: var(--bg); color: #eee; font-family: 'Segoe UI', sans-serif; margin: 0; padding: 20px; }
        .container { max-width: 1100px; margin: auto; }
        
        /* Barre de Navigation */
        .navbar { display: flex; justify-content: center; gap: 12px; background: #1a1a1a; padding: 15px; border-radius: 10px; margin-bottom: 30px; border: 1px solid #222; }
        .navbar a { color: white; text-decoration: none; padding: 10px 22px; border-radius: 6px; font-weight: bold; background: #333; transition: 0.3s; }
        .nav-temp { background: var(--accent) !important; color: #fff !important; }

        .grid { display: grid; grid-template-columns: 400px 1fr; gap: 25px; }
        .card { background: var(--card); padding: 25px; border-radius: 15px; border: 1px solid #222; text-align: center; }
        
        h1 { color: var(--accent); text-align: center; text-transform: uppercase; letter-spacing: 2px; }
        .valeur-digitale { font-size: 3.5rem; font-weight: bold; color: var(--accent); margin-top: -45px; }
        h3 { color: #666; text-transform: uppercase; font-size: 0.8rem; margin-bottom: 10px; }
    </style>
</head>
<body>

<div class="container">
    <div class="navbar">
        <a href="accueil.php"> Accueil</a>
        <a href="temperature.php" class="nav-temp"> Température</a>
        <a href="humidite.php" style="background:#2ecc71; color:black"> Humidité</a>
        <a href="historique.php"> Historique</a>
    </div>

    <h1>SUPERVISION TEMPÉRATURE</h1>
    <div style="text-align:center; color:#555; margin-bottom:20px;">Dernière donnée : Zone <?php echo $zone_actuelle; ?></div>

    <div class="grid">
        <!-- JAUGE DEMI-LUNE -->
        <div class="card">
            <h3>Direct (0-50°C)</h3>
            <canvas id="gaugeTemp"></canvas>
            <div class="valeur-digitale"><?php echo number_format($temp_actuelle, 1); ?>°C</div>
        </div>

        <!-- GRAPHIQUE HISTORIQUE -->
        <div class="card">
            <h3>Historique Récent</h3>
            <div style="height: 320px; width: 100%;">
                <canvas id="chartTemp"></canvas>
            </div>
        </div>
    </div>
</div>

<script>
    // Configuration de la Jauge Semi-Circulaire
    var gauge = new RadialGauge({
        renderTo: 'gaugeTemp',
        width: 380,
        height: 380,
        units: "°C",
        minValue: 0,
        maxValue: 50, // Échelle max 50°C
        value: <?php echo $temp_actuelle; ?>, 
        
        // Paramètres de forme (Demi-Lune)
        startAngle: 90,
        ticksAngle: 180,
        strokeTicks: false,
        
        // Couleurs et Design
        colorPlate: "transparent",
        colorMajorTicks: "#444",
        colorNumbers: "#888",
        majorTicks: ["0","10","20","30","40","50"],
        highlights: [{ "from": 0, "to": 50, "color": "#e67e22" }],
        
        // Aiguille
        needleType: "arrow",
        needleWidth: 4,
        colorNeedle: "#fff",
        valueBox: false,
        borders: false,
        
        animationDuration: 1500,
        animationRule: "decelerate"
    }).draw();

    // Configuration du Graphique
    new Chart(document.getElementById('chartTemp'), {
        type: 'line',
        data: {
            labels: <?php echo json_encode($labels); ?>,
            datasets: [{
                label: 'Température (°C)',
                data: <?php echo json_encode($data); ?>,
                borderColor: '#e67e22',
                backgroundColor: 'rgba(230, 126, 34, 0.1)',
                borderWidth: 4,
                pointRadius: 6,
                fill: true,
                tension: 0.3
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                y: { 
                    beginAtZero: true, 
                    max: 50, // Axe Y bloqué à 50
                    grid: { color: '#222' }, 
                    ticks: { color: '#666' } 
                },
                x: { grid: { display: false }, ticks: { color: '#888' } }
            },
            plugins: { legend: { display: false } }
        }
    });
</script>

</body>
</html>