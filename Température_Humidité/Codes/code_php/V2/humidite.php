<?php
$conn = new mysqli("localhost","root","","supervision_salle_de_sport");

$result = $conn->query("SELECT * FROM mesures_humidite_temperature ORDER BY id DESC LIMIT 1");
$row = $result->fetch_assoc();

$humi = $row['humidite'];
?>

<!DOCTYPE html>
<html>
<head>
    <title>Humidite</title>
    <link rel="stylesheet" href="css/csshumi.css">
    <meta http-equiv="refresh" content="5">
</head>
<body>

<h1>Humidite</h1>

<input type="range" min="0" max="100" value="<?php echo $humi; ?>" disabled>
<p><?php echo $humi; ?> %</p>

<table>
<tr><th>ID</th><th>Humidite</th><th>Date</th></tr>

<?php
$result = $conn->query("SELECT * FROM mesures_humidite_temperature ORDER BY id DESC");

while($row = $result->fetch_assoc()){
    echo "<tr>";
    echo "<td>".$row['id']."</td>";
    echo "<td>".$row['humidite']." %</td>";
    echo "<td>".$row['date']."</td>";
    echo "</tr>";
}
?>

</table>

</body>
</html>