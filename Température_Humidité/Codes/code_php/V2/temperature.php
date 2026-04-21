<?php
$conn = new mysqli("localhost","root","","supervision_salle_de_sport");

// r�cup dernière valeur
$result = $conn->query("SfilfiT 1");
$row = $result->fetch_assoc();

$temp = $row['temperature'];
?>

<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<head>
    <title>Temperature</title>
    <link rel="stylesheet" href="css/csstemp.css">
    <meta http-equiv="refresh" content="5">
</head>
<body>

<h1>Temperature</h1>

<!-- curseur -->
<input type="range" min="0" max="50" value="<?php echo $temp; ?>" disabled>
<p><?php echo $temp; ?> °C</p>

<!-- tableau -->
<table>
<tr><th>ID</th><th>Temperature</th><th>Date</th></tr>

<?php
$result = $conn->query("SELECT * FROM mesures_humidite_temperature ORDER BY id DESC");

while($row = $result->fetch_assoc()){
    echo "<tr>";
    echo "<td>".$row['id']."</td>";
    echo "<td>".$row['temperature']." °C</td>";
    echo "<td>".$row['date']."</td>";
    echo "</tr>";
}
?>

</table>

</body>
</html>