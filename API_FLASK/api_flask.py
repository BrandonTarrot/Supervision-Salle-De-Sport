rom flask import Flask, request, jsonify
import mysql.connector
from datetime import datetime

app = Flask(__name__)

# Configuration de la  base de donnÃ©es Raspberry
db_config = {
    'host': '127.0.0.1',
    'user': 'api',
    'password': 'api',
    'database': 'supervision_salle_de_sport2'
}

@app.route('/api/luminosite', methods=['POST'])
def receive_data():
    data = request.json
    lux = data.get('luminosite')
    # On peut recevoir la zone de l'ESP32 ou la fixer par dÃ©faut
    zone = data.get('id_zone', '1')

    try:
        # Connexion Ã  la base du Raspberry
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()
        # RequÃªte SQL pour ta table : mesures_luminosite (id, luminosite, date, zone)
        sql = "INSERT INTO mesures_luminosite (luminosite, date, id_zone) VALUES (%s, %s, %s)"

        # RÃ©cupÃ©ration de l'horodatage actuel
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

        # RÃ©cupÃ©ration des 20 derniÃ¨res mesures pour ton dashboard PHP
        # On utilise des alias (AS) pour que le PHP retrouve ses noms de variables habituels
        query = "SELECT date AS timestamp, luminosite, id_zone FROM mesures_luminosite ORDER BY date DESC LIMIT 20"
        cursor.execute(query)
        rows = cursor.fetchall()

        cursor.close()
        conn.close()

        return jsonify(rows), 200
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/api/temperature_humidite', methods=['POST'])
def recevoir_donnees():
    try:
        # 1. Extraction des donnees JSON envoyees par l'ESP32
        donnees = request.get_json()
        if not donnees:
            return jsonify({"status": "error", "message": "JSON manquant ou invalide"}), 400

        temp = donnees.get('temperature')
        humi = donnees.get('humidite')
        zone = donnees.get('zone', 1)
        ventilo = donnees.get('ventilateur', 0) # Recupere l'etat (0 ou 1), defaut a 0

        # 2. Date et heure actuelle pour l'insertion
        maintenant = datetime.now()

        print(f"\n[API] Donnees recues -> Temp: {temp}C | Hum: {humi}% | Ventilo: {ventilo}")

        # 3. Connexion et insertion dans la BDD
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()

        # Requete SQL adaptee a tes colonnes
        sql = """INSERT INTO mesures_humidite_temperature
                 (temperature, humidite, id_zone, date, ventilateur)
                 VALUES (%s, %s, %s, %s, %s)"""

        cursor.execute(sql, (temp, humi, zone, maintenant, ventilo))
        conn.commit()

        # Fermeture des acces BDD
        cursor.close()
        conn.close()

        print("[API] Enregistrement reussi dans la BDD Wamp ! ")
        return jsonify({"status": "success", "message": "Donnees inserees"}), 200

    except mysql.connector.Error as err:
        print(f"[ERREUR BDD] : {err}")
        return jsonify({"status": "error", "message": f"Erreur MySQL : {err}"}), 500

    except Exception as e:
        print(f"[ERREUR INTERNE] : {e}")
        return jsonify({"status": "error", "message": str(e)}), 500

@app.route('/api/presence', methods=['POST'])
def receive_presence_data():
    data = request.json

    # Recuperation de la valeur (1 pour mouvement, 0 pour aucun)
    presence = data.get('presence')
    # On recupere la zone sous forme d'entier (1) au lieu d'une chaine
    # Si 'id_zone' n'est pas fourni, on force l'entier 1
    try:
        zone = int(data.get('id_zone', 1))
    except (ValueError, TypeError):
        zone = 1

    try:
        # Connexion a la base du Raspberry
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()

        # Requete SQL pour inserer la presence, la date et la zone numerique
        sql = "INSERT INTO mesures_presence (presence, date, id_zone) VALUES (%s, %s, %s)"

        # Recuperation de l'horodatage actuel
        maintenant = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        # Execution de la requete
        cursor.execute(sql, (presence, maintenant, zone))

        conn.commit()
        cursor.close()
        conn.close()

        print(f"Succes : Presence ({presence}) inseree pour la zone {zone}")
        return jsonify({"status": "success"}), 201

    except Exception as e:
        print(f"Erreur de connexion BDD ou SQL : {e}")
        return jsonify({"status": "error", "message": str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)