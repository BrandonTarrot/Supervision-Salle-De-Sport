<?php require_once('config.php'); ?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Dashboard - Accueil</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>

<div class="container">
    <h1>☀️ Dashboard Luminosité</h1>

    <?php if ($erreur_message): ?>
        <div class="error-banner"><?php echo $erreur_message; ?></div>
    <?php endif; ?>

    <div class="dashboard-header">
        <h2>Les 10 dernières mesures</h2>
        <a href="historique.php" class="btn-history">Voir tout l'historique →</a>
        <a href="graphique.php" class="btn-history" style="background-color: #27ae60; margin-left: 10px;">Voir le Graphique 📈</a>
    </div>

    <table>
        <thead>
            <tr>
                <th>Date</th>
                <th>Intensité (Lux)</th>
                <th>Etat</th>
            </tr>
        </thead>
        <tbody>
            <?php 
            if (!empty($historique)) {
                $top10 = array_slice(array_reverse($historique), 0, 10);
                
                foreach ($top10 as $mesure) {
                    $lux = $mesure['luminosite'] ?? 0;
                    $rowClass = ($lux < 100) ? "status-dark" : (($lux < 500) ? "status-ambient" : "status-bright");
                    $label = ($lux < 100) ? "Sombre" : (($lux < 500) ? "Ambiant" : "Éclairé");
                    $badgeClass = ($lux < 100) ? "badge-dark" : (($lux < 500) ? "badge-ambient" : "badge-bright");
                    
                    // Formatage de la date
                    $dateTexte = "N/A";
                    if (isset($mesure['timestamp'])) {
                        $date = new DateTime($mesure['timestamp']);
                        $dateTexte = $date->format('d/m/Y à H:i');
                    }
                    ?>
                    <tr class="<?php echo $rowClass; ?>">
                        <td><?php echo $dateTexte; ?></td>
                        <td><strong><?php echo number_format($lux, 1); ?> lx</strong></td>
                        <td><span class="badge <?php echo $badgeClass; ?>"><?php echo $label; ?></span></td>
                    </tr>
                <?php 
                } // Fin du foreach
            } else {
                echo "<tr><td colspan='3'>Aucune donnée disponible.</td></tr>";
            } 
            ?>
        </tbody>
    </table>
</div>

</body>
</html>