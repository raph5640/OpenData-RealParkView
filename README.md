# OpenData

## Présentation

Le programme permet de visualiser la disponibilité des parkings en temps réel. Il effectue les actions suivantes :

- Récupère régulièrement des données depuis une [source](https://www.data.gouv.fr/fr/datasets/disponibilite-temps-reel-des-parkings-mel/) et les sauvegarde sous forme de fichier JSON.
  
- Extraction des données :
  - Nom du parking
  - Places disponibles
  - Capacité maximale
  
- Génération d'un histogramme montrant la disponibilité de tout les parkings actuel (Ce graphique est mis a jour lors de chaque collecte de données)

![parking12](https://github.com/raph5640/OpenData/assets/140059828/2f6dda29-b636-41e7-a72e-9774dee36b5d)



- Sauvegarde des données : chaque parking a un fichier .json correspondant dans le répertoire **Data_parkings** contenant un maximum de 13 entrées sauvegardant les 13 dernieres collectes de données (Places_Dispo/Places_Max/Data et heure de la collecte).

![parking_data](https://github.com/raph5640/OpenData/assets/140059828/c99bd791-51a2-4e1f-876a-01933edd0b08)


**Note:** À son lancement, le programme affiche des graphiques pour chaque parking ainsi que le decompte avant la prochaine collecte de donnée **Prochaine collecte dans 9 minutes 58 secondes...** 

![parking9](https://github.com/raph5640/OpenData/assets/140059828/4428276a-29b2-47ad-9fd6-fd123a797e99)


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

1. **Taux_dispo_actuel_TOUT_parkings.png** : Un histogramme montrant la disponibilité de chaque parking de la derniere collecte de donnée.

![parking11](https://github.com/raph5640/OpenData/assets/140059828/10a56e01-b46b-4f86-a431-92f55ce93018)


2. **Nom_du_parking_evolution.png** (exemple de nom) : Ce fichier, situé dans le répertoire **Image_PNG**, est mis à jour à chaque exécution du programme et a chaque collecte de donnée pour afficher l'évolution de la disponibilité du parking correspondant.

![evolution_parking5](https://github.com/raph5640/OpenData/assets/140059828/c0e70f03-a4b7-49dc-be2e-d713b3ba54cd)



# Documentation Technique : OpenData Parking MEL

Executer cette commande : `git clone https://github.com/raph5640/OpenData.git`

Pour visualiser la documentation **Doxygen** depuis le repertoire `/OpenData` : `firefox docs/html/index.html`

![doxygen3](https://github.com/raph5640/OpenData/assets/140059828/cac9bb53-5d9b-4c50-be9f-3d32dedfded6)


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
3. Activer le serveur web lighttpd (BR2_PACKAGE_LIGHTTPD)
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

# Configuration du serveur Web lighttpd avec Buildroot

## 1. Préparation de l'environnement

**Lancement de la machine virtuelle Buildroot** :

`bash
qemu-system-aarch64 -M virt \
-cpu cortex-a57 \
-nographic \
-smp 1 \
-kernel output/images/Image \
-append "root=/dev/vda console=ttyAMA0" \
-netdev user,id=eth0,hostfwd=tcp::2222-:22,hostfwd=tcp::8888-:80 -device virtio-net-device,netdev=eth0 \
-drive file=output/images/rootfs.ext4,if=none,format=raw,id=hd0 \
-device virtio-blk-device,drive=hd00`

## 2. Problèmes initiaux avec lighttpd

En essayant de lancer lighttpd :

`lighttpd -f /etc/lighttpd/lighttpd.conf`

Nous avons rencontré l'erreur indiquant que l'adresse était déjà utilisée.

## 3. Diagnostic du problème

Vérification de l'utilisation du port :

`netstat -tuln | grep :80
lsof -i :80`

## 4. Déplacement des fichiers à servir

Création d'un répertoire pour les fichiers du serveur web :

`mkdir /www`

Déplacement des fichiers nécessaires vers le nouveau répertoire :

`mv /root/Data_parking /www`

`mv /root/Images_PNG /www`

`mv /root/prog_qemu /www`

`mv /root/Images_histograms.html /www`

`mv /root/disponibilite_parkings.json /www`

## 5. Configuration des autorisations

Modification des autorisations pour assurer l'accès aux fichiers :

`chmod -R 755 /www/`

## 6. Configuration de lighttpd

Modification du fichier de configuration de lighttpd pour définir le répertoire racine :

`vi /etc/lighttpd/lighttpd.conf`
Ajout ou modification de la ligne :

**server.document-root = "/www"**

## 7. Redémarrage de lighttpd
Tuer le processus lighttpd précédent et démarrage du serveur :

`kill -9 [PID]
lighttpd -f /etc/lighttpd/lighttpd.conf`

## 8. Test de la configuration
Depuis la machine hôte :

`wget http://localhost:8888/Images_histograms.html`
