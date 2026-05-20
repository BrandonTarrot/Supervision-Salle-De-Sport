<?php
$host = "localhost";
$user = "root";
$pass = "";
$db   = "supervision_salle_de_sport";

$conn = new mysqli($host, $user, $pass, $db);
$result = $conn->query("SELECT * FROM mesures_esp32 ORDER BY id DESC LIMIT 30");
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Historique - Salle de Sport</title>
    <style>
        body { font-family: sans-serif; background: #121212; color: white; padding: 20px; }
        
        /* Barre de Navigation */
        .navbar {
            display: flex;
            justify-content: center;
            gap: 15px;
            margin-bottom: 30px;
            padding: 15px;
            background: #1e1e1e;
            border-radius: 10px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
        }
        .navbar a {
            color: white;
            text-decoration: none;
            padding: 10px 20px;
            border-radius: 5px;
            transition: 0.3s;
            font-weight: bold;
            font-size: 0.95rem;
            display: flex;
            align-items: center;
            gap: 8px;
        }
        .nav-home { border: 1px solid #555; }
        .nav-temp { background: #ff5722; }
        .nav-humi { background: #2196f3; }
        .nav-hist { background: #333; border: 1px solid #2196f3; box-shadow: inset 0 0 10px rgba(33, 150, 243, 0.2); }
        
        .navbar a:hover { 
            opacity: 0.9; 
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0,0,0,0.4);
        }

        /* Style Tableau */
        .content-container { max-width: 1000px; margin: auto; }
        table { width: 100%; border-collapse: collapse; background: #1e1e1e; border-radius: 10px; overflow: hidden; box-shadow: 0 10px 30px rgba(0,0,0,0.5); }
        th, td { padding: 15px; text-align: left; border-bottom: 1px solid #333; }
        th { background: #252525; color: #2196f3; text-transform: uppercase; font-size: 0.85rem; letter-spacing: 1px; }
        tr:hover { background: #252525; }
        
        .temp-cell { color: #ff5722; font-weight: bold; }
        .humi-cell { color: #2196f3; font-weight: bold; }
        h1 { text-align: center; margin-bottom: 25px; letter-spacing: 2px; text-transform: uppercase; }
        
        .empty-msg { text-align: center; padding: 40px; color: #777; font-style: italic; }
    </style>
</head>
<body>

    <!-- Barre de Navigation Connectée -->
    <div class="navbar">
        <a href="index.php" class="nav-home"> Accueil</a>
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
                        <td class="temp-cell"><?php echo number_format($row['temperature'], 1); ?> °C</td>
                        <td class="humi-cell"><?php echo number_format($row['humidite'], 1); ?> %</td>
                        <td><?php echo date('d/m/Y H:i:s', strtotime($row['date'])); ?></td>
                    </tr>
                    <?php endwhile; ?>
                <?php else: ?>
                    <tr>
                        <td colspan="4" class="empty-msg">Aucune donnée disponible dans l'historique.</td>
                    </tr>
                <?php endif; ?>
            </tbody>
        </table>
    </div>

</body>
</html>