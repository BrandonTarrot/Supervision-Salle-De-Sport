<?php
// --- CONFIGURATION CONNEXION RASPBERRY ---
$host = "172.19.6.120"; 
$user = "api";
$pass = "api"; 
$db   = "supervision_salle_de_sport2";

$conn = new mysqli($host, $user, $pass, $db);

// Vérification de la connexion
if ($conn->connect_error) {
    die("Erreur de connexion : " . $conn->connect_error);
}

// Requête pour les 10 DERNIÈRES VALEURS
$result = $conn->query("SELECT * FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 10");
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Historique - Salle de Sport</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; background: #0d0d0d; color: white; padding: 20px; }
        
        /* Barre de Navigation */
        .navbar {
            display: flex;
            justify-content: center;
            gap: 15px;
            margin-bottom: 30px;
            padding: 15px;
            background: #1a1a1a;
            border-radius: 10px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.5);
        }
        .navbar a {
            color: white;
            text-decoration: none;
            padding: 10px 20px;
            border-radius: 5px;
            transition: 0.3s;
            font-weight: bold;
            font-size: 0.95rem;
        }
        .nav-home { background: #333; }
        .nav-temp { background: #e67e22; }
        .nav-humi { background: #2ecc71; color: black; }
        .nav-hist { border: 1px solid #2ecc71; background: #1a1a1a; }
        
        .navbar a:hover { transform: translateY(-2px); opacity: 0.8; }

        /* Style Tableau */
        .content-container { max-width: 1000px; margin: auto; }
        table { width: 100%; border-collapse: collapse; background: #161616; border-radius: 12px; overflow: hidden; }
        th, td { padding: 15px; text-align: left; border-bottom: 1px solid #222; }
        th { background: #222; color: #2ecc71; text-transform: uppercase; font-size: 0.85rem; }
        tr:hover { background: #1f1f1f; }
        
        .temp-cell { color: #e67e22; font-weight: bold; }
        .humi-cell { color: #2ecc71; font-weight: bold; }
        h1 { text-align: center; margin-bottom: 25px; letter-spacing: 2px; text-transform: uppercase; color: #2ecc71; }
        
        .empty-msg { text-align: center; padding: 40px; color: #777; font-style: italic; }
    </style>
</head>
<body>

    <div class="navbar">
        <a href="accueil.php" class="nav-home"> Accueil</a>
        <a href="temperature.php" class="nav-temp"> Température</a>
        <a href="humidite.php" class="nav-humi"> Humidité</a>
        <a href="historique.php" class="nav-hist"> Historique</a>
    </div>

    <div class="content-container">
        <h1>Journal des Mesures</h1>
        
        <table>
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Zone</th>
                    <th>Température</th>
                    <th>Humidité</th>
                    <th>Date & Heure</th>
                </tr>
            </thead>
            <tbody>
                <?php if ($result && $result->num_rows > 0): ?>
                    <?php while($row = $result->fetch_assoc()): ?>
                    <tr>
                        <td>#<?php echo $row['id']; ?></td>
                        <td>Zone <?php echo $row['zone']; ?></td>
                        <td class="temp-cell"><?php echo number_format($row['temperature'], 1); ?> °C</td>
                        <td class="humi-cell"><?php echo number_format($row['humidite'], 1); ?> %</td>
                        <td><?php echo date('d/m/Y H:i:s', strtotime($row['date'])); ?></td>
                    </tr>
                    <?php endwhile; ?>
                <?php else: ?>
                    <tr>
                        <td colspan="5" class="empty-msg">Aucune donnée trouvée dans la table 'mesures_humidite_temperature'.</td>
                    </tr>
                <?php endif; ?>
            </tbody>
        </table>
    </div>

</body>
</html>