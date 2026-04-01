<?php require_once('config.php'); ?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Graphique de Luminosité</title>
    <link rel="stylesheet" href="style.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>

<div class="container">
    <a href="dashboard_lux.php" style="text-decoration: none; color: #3498db;">← Retour au Dashboard</a>
    <h1>📈 Évolution de la Luminosité</h1>
    <p style="text-align:center; color: #7f8c8d;">Visualisation des 10 derniers relevés</p>

    <div class="graph-container" style="background: white; padding: 20px; border-radius: 10px; margin-top: 20px;">
        <canvas id="luxChart"></canvas>
    </div>

    <?php 
    // Préparation des données PHP pour JS
    $labels = [];
    $valeurs = [];
    if (!empty($historique)) {
        // On prend les 10 derniers et on les remet dans l'ordre chronologique
        $top10 = array_slice(array_reverse($historique), 0, 10);
        foreach (array_reverse($top10) as $m) {
            $date = new DateTime($m['timestamp'] ?? 'now');
            $labels[] = $date->format('H:i');
            $valeurs[] = $m['luminosite'] ?? 0;
        }
    }
    ?>

    <script>
    const ctx = document.getElementById('luxChart').getContext('2d');
    new Chart(ctx, {
        type: 'line',
        data: {
            labels: <?php echo json_encode($labels); ?>,
            datasets: [{
                label: 'Luminosité (Lux)',
                data: <?php echo json_encode($valeurs); ?>,
                borderColor: '#f1c40f',
                backgroundColor: 'rgba(241, 196, 15, 0.1)',
                fill: true,
                tension: 0.4
            }]
        },
        options: {
            responsive: true,
            scales: {
                y: { beginAtZero: true, title: { display: true, text: 'Lux' } }
            }
        }
    });
    </script>
</div>

</body>
</html>