<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Accueil - Supervision Salle de Sport</title>
    <style>
        body {
            font-family: 'Segoe UI', sans-serif;
            background: #121212;
            color: white;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
        }
        h1 { margin-bottom: 40px; text-transform: uppercase; letter-spacing: 2px; }
        .menu { display: flex; gap: 25px; flex-wrap: wrap; justify-content: center; }
        .card {
            background: #1e1e1e;
            padding: 30px;
            border-radius: 15px;
            width: 200px;
            text-align: center;
            transition: 0.3s;
            border: 1px solid #333;
            cursor: pointer;
        }
        .card:hover { transform: translateY(-10px); border-color: #00bcd4; }
        .card a { text-decoration: none; color: white; font-weight: bold; font-size: 1.1em; }
        .icon { font-size: 3em; display: block; margin-bottom: 15px; }
    </style>
</head>
<body>
    <h1>Supervision Salle de Sport</h1>
    
    <div class="menu">
        <div class="card" onclick="window.location.href='temperature.php'">
            <span class="icon"></span>
            <a href="temperature.php">Température</a>
        </div>
        <div class="card" onclick="window.location.href='humidite.php'">
            <span class="icon"></span>
            <a href="humidite.php">Humidité</a>
        </div>
        <div class="card" onclick="window.location.href='historique.php'">
            <span class="icon"></span>
            <a href="historique.php">Historique</a>
        </div>
    </div>
</body>
</html>