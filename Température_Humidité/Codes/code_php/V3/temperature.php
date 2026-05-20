<?php
// --- CONFIGURATION BDD ---
$host = "localhost"; 
$user = "root";
$pass = ""; 
$db   = "supervision_salle_de_sport2";

$conn = new mysqli($host, $user, $pass, $db);

// Vérification de la connexion
if ($conn->connect_error) {
    die("Erreur de connexion : " . $conn->connect_error);
}

// 1. Récupération de la dernière température pour la jauge
$result_last = $conn->query("SELECT temperature FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 1");
$temp_actuelle = 0;
if ($result_last && $row_last = $result_last->fetch_assoc()) {
    $temp_actuelle = $row_last['temperature'];
}

// 2. Récupération des données pour le graphique (20 dernières mesures)
$query_chart = "SELECT temperature, date_mesure FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 20";
$result_chart = $conn->query($query_chart);

$labels = [];
$temps = [];

while($row = $result_chart->fetch_assoc()){
    // Formatage de l'heure pour l'axe X (HH:mm)
    $labels[] = date("H:i", strtotime($row['date_mesure']));
    $temps[] = $row['temperature'];
}

// Inversion pour l'ordre chronologique sur le graphique
$labels = array_reverse($labels);
$temps = array_reverse($temps);
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Supervision Température - Gym Monitor</title>
    
    <!-- Bibliothèques JS -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.rawgit.com/Mikhus/canvas-gauges/master/gauge.min.js"></script>
    
    <!-- Actualisation automatique toutes les 10 secondes -->
    <meta http-equiv="refresh" content="10">
    
    <style>
        :root {
            --bg-color: #0f0f0f;
            --card-bg: #1a1a1a;
            --text-main: #ffffff;
            --text-muted: #888888;
            --accent: #00e5ff; /* Cyan néon */
            --danger: #ff5252;
            --success: #00e676;
            --border: #333333;
        }

        body {
            font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background-color: var(--bg-color);
            color: var(--text-main);
            margin: 0;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .dashboard {
            width: 100%;
            max-width: 1100px;
            display: flex;
            flex-direction: column;
            gap: 25px;
        }

        /* --- BARRE DE NAVIGATION --- */
        .navbar {
            display: flex;
            justify-content: center;
            gap: 15px;
            margin-bottom: 20px;
            padding: 15px;
            background: var(--card-bg);
            border-radius: 12px;
            border: 1px solid var(--border);
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
        .nav-temp { background: var(--accent); color: #000; border: 1px solid white; } /* Actif */
        .nav-humi { background: #2ecc71; } /* Vert */
        .nav-hist { background: #444; border: 1px solid var(--accent); }
        
        .navbar a:hover { 
            opacity: 0.9; 
            transform: translateY(-2px);
        }

        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-bottom: 1px solid var(--border);
            padding-bottom: 15px;
        }

        .header h1 {
            margin: 0;
            font-size: 1.5rem;
            letter-spacing: 1px;
            color: var(--accent);
        }

        .status-dot {
            height: 10px;
            width: 10px;
            background-color: var(--success);
            border-radius: 50%;
            display: inline-block;
            margin-right: 5px;
            box-shadow: 0 0 8px var(--success);
        }

        .top-row {
            display: grid;
            grid-template-columns: 1fr 2fr;
            gap: 20px;
        }

        @media (max-width: 800px) {
            .top-row { grid-template-columns: 1fr; }
        }

        .card {
            background-color: var(--card-bg);
            border-radius: 16px;
            padding: 25px;
            border: 1px solid var(--border);
            box-shadow: 0 10px 30px rgba(0,0,0,0.5);
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .card h2 {
            margin: 0 0 20px 0;
            font-size: 0.9rem;
            color: var(--text-muted);
            text-transform: uppercase;
            align-self: flex-start;
        }

        .temp-display {
            font-size: 3.5rem;
            font-weight: 800;
            color: var(--accent);
            margin-top: -15px;
        }

        .temp-unit {
            font-size: 1.2rem;
            vertical-align: super;
            color: var(--text-muted);
        }

        /* Styles du tableau */
        .history-card {
            align-items: stretch;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 10px;
        }

        th {
            text-align: left;
            color: var(--text-muted);
            font-weight: 600;
            padding: 15px 10px;
            border-bottom: 1px solid var(--border);
        }

        td {
            padding: 15px 10px;
            border-bottom: 1px solid #222;
            font-size: 0.95rem;
        }

        tr:last-child td { border-bottom: none; }

        .btn-nav-back {
            padding: 12px 30px;
            background: transparent;
            border: 1px solid var(--accent);
            color: var(--accent);
            text-decoration: none;
            border-radius: 50px;
            font-weight: 600;
            transition: all 0.3s ease;
            margin-top: 20px;
            display: inline-block;
        }

        .btn-nav-back:hover {
            background: var(--accent);
            color: #000;
            box-shadow: 0 0 15px var(--accent);
        }

        .chart-container {
            width: 100%;
            height: 280px;
        }
    </style>
</head>
<body>

    <div class="dashboard">
        <!-- NOUVELLE BARRE DE NAVIGATION -->
        <div class="navbar">
            <a href="acceuil.html" class="nav-home">🏠 Accueil</a>
            <a href="temperature.php" class="nav-temp">🌡️ Température</a>
            <a href="humidite.php" class="nav-humi">💧 Humidité</a>
            <a href="historique.php" class="nav-hist">📜 Historique</a>
        </div>

        <div class="header">
            <h1>SYSTEME DE SUPERVISION</h1>
            <div><span class="status-dot"></span> LIVE</div>
        </div>

        <div class="top-row">
            <!-- JAUGE CIRCULAIRE -->
            <div class="card">
                <h2>Température Actuelle</h2>
                <canvas id="gaugeRadial"></canvas>
                <div class="temp-display">
                    <?php echo number_format($temp_actuelle, 1); ?><span class="temp-unit">°C</span>
                </div>
            </div>

            <!-- GRAPHIQUE LINEAIRE -->
            <div class="card">
                <h2>Historique Temporel</h2>
                <div class="chart-container">
                    <canvas id="tempChart"></canvas>
                </div>
            </div>
        </div>

        <!-- TABLEAU DE DONNEES -->
        <div class="card history-card">
            <h2>Derniers Relevés</h2>
            <table>
                <thead>
                    <tr>
                        <th>ID</th>
                        <th>Valeur</th>
                        <th>Zone</th>
                        <th>Horodatage</th>
                    </tr>
                </thead>
                <tbody>
                    <?php 
                    $result_table = $conn->query("SELECT * FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 6");
                    if($result_table && $result_table->num_rows > 0) {
                        while($row = $result_table->fetch_assoc()){
                            echo "<tr>";
                            echo "<td>#{$row['id']}</td>";
                            echo "<td style='color:var(--accent); font-weight:bold;'>{$row['temperature']} °C</td>";
                            echo "<td>" . ($row['zone'] ?? 'Salle Principale') . "</td>";
                            echo "<td>{$row['date_mesure']}</td>";
                            echo "</tr>";
                        }
                    } else {
                        echo "<tr><td colspan='4' style='text-align:center;'>Aucune donnée disponible</td></tr>";
                    }
                    ?>
                </tbody>
            </table>
            
            <div style="text-align: center;">
                <a href="acceuil.html" class="btn-nav-back">RETOUR AU TABLEAU DE BORD</a>
            </div>
        </div>
    </div>

    <script>
        // --- INITIALISATION DE LA JAUGE ---
        var gauge = new RadialGauge({
            renderTo: 'gaugeRadial',
            width: 220,
            height: 220,
            units: "°C",
            minValue: 0,
            maxValue: 50,
            majorTicks: ["0","10","20","30","40","50"],
            minorTicks: 2,
            strokeTicks: true,
            highlights: [
                { "from": 0, "to": 18, "color": "rgba(0, 229, 255, .1)" },
                { "from": 18, "to": 28, "color": "rgba(0, 230, 118, .2)" },
                { "from": 28, "to": 50, "color": "rgba(255, 82, 82, .2)" }
            ],
            colorPlate: "transparent",
            colorMajorTicks: "#777",
            colorMinorTicks: "#444",
            colorNumbers: "#eee",
            borderShadowWidth: 0,
            borders: false,
            needleType: "arrow",
            needleWidth: 3,
            needleCircleSize: 7,
            needleCircleOuter: true,
            needleCircleInner: false,
            animationDuration: 1500,
            animationRule: "decelerate",
            value: <?php echo $temp_actuelle; ?>
        }).draw();

        // --- INITIALISATION DU GRAPHIQUE ---
        const ctx = document.getElementById('tempChart').getContext('2d');
        const tempChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: <?php echo json_encode($labels); ?>,
                datasets: [{
                    label: 'Température (°C)',
                    data: <?php echo json_encode($temps); ?>,
                    borderColor: '#00e5ff',
                    backgroundColor: 'rgba(0, 229, 255, 0.1)',
                    borderWidth: 3,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 4,
                    pointBackgroundColor: '#00e5ff',
                    pointBorderColor: '#1a1a1a',
                    pointBorderWidth: 2
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: { display: false }
                },
                scales: {
                    y: {
                        beginAtZero: false,
                        grid: { color: '#222' },
                        ticks: { color: '#888' }
                    },
                    x: {
                        grid: { color: '#222' },
                        ticks: { color: '#888' }
                    }
                }
            }
        });
    </script>
</body>
</html>