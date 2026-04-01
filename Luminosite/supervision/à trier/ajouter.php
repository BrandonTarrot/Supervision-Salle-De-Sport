<?php require_once 'config.php'; ?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Ajouter une mesure</title>
    <style>
        body { font-family: sans-serif; background: #f4f4f4; padding: 50px; }
        .form-card { background: white; padding: 20px; max-width: 400px; margin: auto; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.2); }
        input, select { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; }
        button { width: 100%; padding: 10px; background: #28a745; color: white; border: none; border-radius: 4px; cursor: pointer; }
    </style>
</head>
<body>

<div class="form-card">
    <h2>Nouvelle Mesure</h2>
    <form action="" method="POST">
        <input type="number" step="0.1" name="temp" placeholder="Température (°C)" required>
        <input type="number" step="0.1" name="hum" placeholder="Humidité (%)" required>
        <select name="zone">
            <option value="1">Zone A (1)</option>
            <option value="2">Zone B (2)</option>
        </select>
        <button type="submit" name="valider">Enregistrer</button>
    </form>

    <?php
    if(isset($_POST['valider'])){
        $t = $_POST['temp'];
        $h = $_POST['hum'];
        $z = $_POST['zone'];
        $d = date("Y-m-d H:i:s");

        $sql = "INSERT INTO mesures_humidite_temperature (temperature, humidite, date, zone) VALUES ('$t', '$h', '$d', '$z')";
        
        if ($conn->query($sql) === TRUE) {
            echo "<p style='color:green;'>Donnée ajoutée ! <a href='index.php'>Voir la liste</a></p>";
        } else {
            echo "Erreur: " . $conn->error;
        }
    }
    ?>
</div>

</body>
</html>