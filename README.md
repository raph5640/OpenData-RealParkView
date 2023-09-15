# OpenData

- Le programme récupère régulièrement des données sur les parkings depuis une URL définie et les sauvegarde localement en tant que fichier JSON.

- Une fois le fichier téléchargé, le programme extrait des données pertinentes concernant le nom du parking, le nombre de places disponibles et la capacité maximale.

- Il génère un histogramme montrant le pourcentage de disponibilité de tous les parkings.

- De plus, pour chaque parking, le programme génère un histogramme montrant le pourcentage de disponibilité et le sauvegarde dans le répertoire **Image_PNG**.

- Les données de disponibilité de chaque parking sont également sauvegardées dans un fichier JSON dans le répertoire **Data_parkings**.

**Ce programme est conçu pour visualiser la disponibilité des parkings en temps réel, offrant à la fois une vue d'ensemble et des détails spécifiques sur tout les parkings.**
![parking6](https://github.com/raph5640/OpenData/assets/140059828/6d3a7900-30b2-4740-8809-f76cad3799ef)


Sujet choisi : Disponibilité temps réel des parkings MEL
https://www.data.gouv.fr/fr/datasets/disponibilite-temps-reel-des-parkings-mel/

Deux fichiers sont proposés .json et .csv. 

1) Utilisation de l'URL stable pour le fichier .json : 
utiliser cette URL : https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false

2) Téléchargement du fichier à partir de l'URL: 
On utilise `libcurl` pour télécharger le fichier .json localement.

3) Parser le fichier .JSON
On parse le fichier pour extraire les données necessaires 

4) Génération du graphique histogramme
On génére un graphique a partir des données récupéré dans une structure avec la bibliothèque GD.

## OBJECTIF : 
- Deux fichiers sont généré : 

- **pourcentage_place_disponible.png** qui est un histogramme qui affiche le pourcentage de disponibilité de chaque parking

![parking5](https://github.com/raph5640/OpenData/assets/140059828/fa265f8f-657a-4e08-ab75-4ef668ea252e)

### Collecte de donnée en temps réel

À chaque exécution du programme prog_qemu ou prog_debian, nous traçons l'évolution du taux de disponibilité de chaque parking à l'aide d'un histogramme. Ce graphique, nommé 'Nom_du_parking_evolution.png', est **actualisé automatiquement toutes les 20 minutes**, reflétant la disponibilité du parking à chaque intervalle dans le repertoire **Image_PNG**

- **'Parking Nouveau Siecle_evolution_taux_disponibilite.png'** : 

![evolution_parking5](https://github.com/raph5640/OpenData/assets/140059828/c0e70f03-a4b7-49dc-be2e-d713b3ba54cd)



# Documentation Technique : OpenData Parking MEL

Executer cette commande : `git clone https://github.com/raph5640/OpenData.git`

## 1. Présentation du projet

- **Fonction principale** : Ce programme vise à récupérer des données en temps réel sur les parkings depuis une URL spécifiée et à les afficher sous forme d'histogrammes. Les histogrammes montrent la disponibilité actuelle des parkings et l'évolution de la disponibilité pour un parking spécifique.
- **Source des données** : https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false

## 2. Dépendances 

- **libcurl** : Pour télécharger le fichier .json depuis l'URL
- **nlohmann/json.hpp** : Une bibliothèque JSON pour C++ utilisée pour parser le fichier .json
- **gd** : Bibliothèque pour la génération de graphiques.

## 3. Compilation

### Debian:

1. Cloner le dépôt: `git clone https://github.com/raph5640/OpenData.git`
2. Accéder au répertoire: `cd OpenData/`
3. Télécharger et installer la dépendance JSON: `git clone https://github.com/nlohmann/json` ou `sudo apt install ljsoncpp`
4. Compiler: `g++ -o prog_debian main.cpp -lgd -lcurl -ljsoncpp -I/home/raphael/json/include`
5. Exécuter: `./prog_debian`

### Buildroot avec QEMU:

1. Lancer `make xconfig`
2. Activez la bibliothèque `libcurl`.
3. Ajoutez `json-for-modern-cpp` avec le flag : **BR2_PACKAGE_JSON_FOR_MODERN_CPP**
4. Intégrez la bibliothèque `gd` : **BR2_PACKAGE_GD** et activez `gdtopng`.
5. Cochez `Enable C++ support`.
6. Lancez `make`.
7. Pour la compilation croisée: `~/buildroot-2023.08/output/host/bin/aarch64-buildroot-linux-gnu-g++ ~/OpenData/main.cpp -o ~/OpenData/prog_qemu -lgd -lcurl -lstdc++fs`

## 4. Transfert et Exécution sur Buildroot

1. Démarrer Buildroot avec `./go` ou `./start_buildroot.sh`.
2. Transférer prog_emu : `scp raphael@10.0.3.15:/home/raphael/OpenData/prog_qemu /root/` (À exécuter depuis Buildroot)
3. Dans Buildroot, allez à **/root/** et exécutez `./prog_qemu`.

## 5. Annexes

- **Images** : Des exemples d'images générées peuvent être consultées directement sur le dépôt GitHub.
