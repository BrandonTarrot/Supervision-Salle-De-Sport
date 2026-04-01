from flask import Flask, request, jsonify
import mysql.connector
from datetime import datetime

app = Flask(__name__)

# --- Configuration de la base de données Raspberry ---
db_config = {
    'host': '127.0.0.1',  # L'adresse IP du Raspberry Pi
    'user': 'api',
    'password': 'api',
    'database': 'supervision_salle_de_sport2'
}

@app.route('/api/luminosite', methods=['POST'])
def receive_data():
    data = request.json
    lux = data.get('luminosite')
    zone = data.get('zone', '1')

    try:
        # Connexion à la bdd du Raspberry
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()

        # Requête SQL pour ta table mesures_luminosite
        sql = "INSERT INTO mesures_luminosite (luminosite, date, zone) VALUES (%s, %s, %s)"

        # Récupération de l'heure actuel
        maintenant = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        cursor.execute(sql, (lux, maintenant, zone))

        conn.commit()
        cursor.close()
        conn.close()

        print(f"SuccÃ¨s : {lux} lx insÃ©rÃ© en base pour la zone {zone}")
        return jsonify({"status": "success"}), 201

    except Exception as e:
        print(f"Erreur de connexion BDD : {e}")
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/api/luminosite/historique', methods=['GET'])
def get_historique():
    try:
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor(dictionary=True)

        # Récupération des 10 dernières mesures pour le dashboard PHP
        # On utilise des alias (AS) pour que le PHP retrouve ses noms de variables habituels
        query = "SELECT date AS timestamp, luminosite, zone FROM mesures_luminosite ORDER BY date DESC LIMIT 20"
        cursor.execute(query)
        rows = cursor.fetchall()

        cursor.close()
        conn.close()

        return jsonify(rows), 200
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

if __name__ == '__main__':
    # host='0.0.0.0' permet Ã  l'ESP32 de communiquer avec le script via le rÃ©seau x
    app.run(host='0.0.0.0', port=5000, debug=True)
