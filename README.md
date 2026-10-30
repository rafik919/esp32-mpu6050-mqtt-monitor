# ESP32 MPU6050 MQTT Monitor

Projet IoT sur ESP32 qui lit l’accélération et la température d’un capteur MPU6050, puis publie les mesures en temps réel avec MQTT.

Le projet est prêt à être simulé avec [Wokwi](https://wokwi.com/).

## Fonctionnalités

- Lecture de l’accélération sur les axes X, Y et Z
- Lecture de la température interne du MPU6050
- Connexion Wi-Fi dans la simulation Wokwi
- Publication MQTT des mesures toutes les secondes
- Pilote MPU6050 réutilisable écrit en C

## Matériel et connexions

| Composant | ESP32 |
| --- | --- |
| MPU6050 VCC | 3V3 |
| MPU6050 GND | GND |
| MPU6050 SDA | GPIO 21 |
| MPU6050 SCL | GPIO 22 |

Le schéma de câblage est disponible dans `diagram.json`.

## Simulation Wokwi

1. Créez un projet ESP32 dans Wokwi.
2. Importez les fichiers de ce dépôt.
3. Ajoutez la bibliothèque `PubSubClient` si Wokwi ne l’installe pas automatiquement.
4. Lancez la simulation et ouvrez le moniteur série.

Les mesures sont envoyées sur le topic MQTT défini dans `sketch.ino`.

## Structure du projet

| Fichier | Rôle |
| --- | --- |
| `sketch.ino` | Programme principal ESP32, Wi-Fi et MQTT |
| `mpu6050.h` | Interface du pilote du capteur |
| `mpu6050.c` | Implémentation du pilote MPU6050 |
| `diagram.json` | Schéma de câblage Wokwi |
| `libraries.txt` | Dépendances Wokwi |

## Sécurité

Le broker MQTT public est adapté à la démonstration. Pour une utilisation réelle, utilisez un broker sécurisé, un topic unique et des identifiants protégés.
