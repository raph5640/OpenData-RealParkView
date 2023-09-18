# OpenData

## Présentation

Le programme permet de visualiser la disponibilité des parkings en temps réel. Il effectue les actions suivantes :

- Récupère régulièrement des données depuis une [source](https://www.data.gouv.fr/fr/datasets/disponibilite-temps-reel-des-parkings-mel/) et les sauvegarde sous forme de fichier JSON.
  
- Extraction des données :
  - Nom du parking
  - Places disponibles
  - Capacité maximale
  
- Génération d'histogrammes montrant la disponibilité des parkings.

- Sauvegarde des données : chaque parking a un fichier correspondant dans le répertoire **Data_parkings**.

![parking6](https://github.com/raph5640/OpenData/assets/140059828/6d3a7900-30b2-4740-8809-f76cad3799ef)

**Note:** À son lancement, le programme affiche des graphiques pour chaque parking. 

![parking_html](https://github.com/raph5640/OpenData/assets/140059828/a01701fd-2abb-4656-aa22-c8c67635f6ec)

De plus, le terminal passe en mode Links pour permettre une visualisation graphique des fichiers générés.

![parking_html2](https://github.com/raph5640/OpenData/assets/140059828/db29a1ba-13d9-40c3-9828-f56d420e929f)

## Détails Techniques

### Source des Données

- Sujet choisi : [Disponibilité temps réel des parkings MEL](https://www.data.gouv.fr/fr/datasets/disponibilite-temps-reel-des-parkings-mel/)
- Formats disponibles : .json et .csv.
- URL stable pour le fichier .json : [Lien direct](https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false)

### Méthodologie

- **Téléchargement** : Utilise `libcurl` pour télécharger le fichier .json.
- **Parsing** : Le fichier .JSON est ensuite parsé pour en extraire les données nécessaires.
- **Génération du graphique** : Un histogramme est créé à partir des données récupérées, en utilisant la bibliothèque GD.

### Objectif

Deux fichiers principaux sont générés :

1. **pourcentage_place_disponible.png** : Un histogramme montrant la disponibilité de chaque parking.

![parking5](https://github.com/raph5640/OpenData/assets/140059828/fa265f8f-657a-4e08-ab75-4ef668ea252e)

2. **Nom_du_parking_evolution.png** (exemple de nom) : Ce fichier, situé dans le répertoire **Image_PNG**, est mis à jour à chaque exécution du programme et a chaque collecte de donnée pour afficher l'évolution de la disponibilité du parking correspondant.

![evolution_parking5](https://github.com/raph5640/OpenData/assets/140059828/c0e70f03-a4b7-49dc-be2e-d713b3ba54cd)



# Documentation Technique : OpenData Parking MEL

Executer cette commande : `git clone https://github.com/raph5640/OpenData.git`

Pour visualiser la documentation **Doxygen** depuis le repertoire `/OpenData` : `firefox docs/html/index.html`

## 1. Présentation du projet

- **Fonction principale** : Ce programme vise à récupérer des données en temps réel sur les parkings depuis une URL spécifiée et à les afficher sous forme d'histogrammes. Les histogrammes montrent la disponibilité actuelle des parkings et l'évolution de la disponibilité pour chaque parking.
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
4. **Compiler: `g++ -o prog_debian main.cpp histogram.cpp datamanager.cpp -lgd -lcurl -ljsoncpp -I/home/raphael/json/include`**
5. Exécuter: `./prog_debian`

### Buildroot avec QEMU:

1. Lancer `make xconfig` depuis votre repertoire ou est installé buildroot `cd /home/raphael/buildroot-2023.08`
2. Activez la bibliothèque `libcurl`.
3. Activer la bibliotheque `links` (BR2_PACKAGE_LINKS)
4. Ajoutez `json-for-modern-cpp` et `libjsoncpp` avec le flag : **BR2_PACKAGE_JSON_FOR_MODERN_CPP et BR2_PACKAGE_LIBJSON** 
5. Intégrez la bibliothèque `gd` : **BR2_PACKAGE_GD** et activez `gdtopng`.
6. Cochez `Enable C++ support`.
7. Lancez `make`.
8. **Pour la compilation croisée: `~/buildroot-2023.08/output/host/bin/aarch64-buildroot-linux-gnu-g++ ~/OpenData/main.cpp ~/OpenData/histogram.cpp ~/OpenData/datamanager.cpp -o ~/OpenData/prog_qemu -lgd -lcurl -lstdc++fs -ljsoncpp -I/home/raphael/json/include`**

## 4. Transfert et Exécution sur Buildroot

1. Démarrer Buildroot avec `./go` ou `./start_buildroot.sh`.
2. Transférer prog_emu : `scp raphael@10.0.3.15:/home/raphael/OpenData/prog_qemu /root/` (À exécuter depuis Buildroot)
3. Dans Buildroot, allez à **/root/** et exécutez `./prog_qemu`.

## 5. Annexes

- **Images** : Des exemples d'images générées peuvent être consultées directement sur le dépôt GitHub dans le repertoire Images_PNG.
