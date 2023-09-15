# OpenData

Le programme récupère régulièrement des données sur les parkings depuis une URL définie et les sauvegarde localement en tant que fichier JSON.

Une fois le fichier téléchargé, le programme extrait des données pertinentes concernant le nom du parking, le nombre de places disponibles et la capacité maximale.

Il génère un histogramme montrant le pourcentage de disponibilité de tous les parkings.

En outre, à chaque démarrage, le programme choisit un parking aléatoirement et génère un histogramme évolutif montrant la disponibilité de ce parking particulier toutes les 5 minutes.

**Ce programme est conçu pour visualiser la disponibilité des parkings en temps réel, offrant à la fois une vue d'ensemble et des détails spécifiques sur un parking sélectionné.**
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

histogramme : calculer le pourcentage de disponibilité de chaque parking

![parking5](https://github.com/raph5640/OpenData/assets/140059828/fa265f8f-657a-4e08-ab75-4ef668ea252e)



## Execution :

Executer cette commande : `git clone https://github.com/raph5640/OpenData.git`

### Debian compilation :

Telecharger et installer la nlohmann/json.hpp. C'est la bibliothèque JSON pour C++ par Niels Lohmann faites un `git clone https://github.com/nlohmann/json` ET/OU `sudo apt install ljsoncpp` (au choix)

Compiler votre programme :


1) Placez vous dans le repertoire OpenData : `cd OpenData/`

2) Compiler votre programme a partir du main.cpp en faisant l'edition de lien avec les biliotheques gd et json : `g++ -o prog_debian main.cpp -lgd -lcurl -ljsoncpp -I/home/raphael/json/include` ou `g++ -o prog_debian main.cpp -lgd -lcurl -ljsoncpp`

3) Lancer votre programme : `./prog_debian`

### QEMU Compilation (buildroot)
1) Faire un `make xconfig`
2) Assurez-vous que la bibliothèque `libcurl` est activée et construite pour votre cible dans Buildroot
3) Ajouter la bibliotheque `json-for-modern-cpp` : BR2_PACKAGE_JSON_FOR_MODERN_CPP
4) Ajouter la bibliotheque `gd` : BR2_PACKAGE_GD -> Puis activer `gdtopng` (Pour la conversion de gd vers png afin de pouvoir générer une image .png)
5) Assurez-vous que `Enable C++ support` est sélectionné/coché.
6) Faire un `make`

7) **compilation croise pour buildroot depuis votre machine** (debian) : `~/buildroot-2023.08/output/host/bin/aarch64-buildroot-linux-gnu-g++ ~/OpenData/main.cpp -o ~/OpenData/prog_qemu -lgd -lcurl -lstdc++fs`

8) Lancer votre machine build-root avec votre shell-script `./go` ou `./start_buildroot.sh`

9) Transfert du prog_emu compilé sur la machine debian vers la machine buildroot avec la commande suivante qui **doit être éxécuté a l'intérieur de la machine buildroot** : `scp raphael@10.0.3.15:/home/raphael/OpenData/prog_qemu /root/`

#### Une fois dans la machine buildroot et le prog_emu transférer faire ceci :

1)  Allez dans le répertoire /root/: `cd /root/`
2)  Listez les fichiers pour voir si prog_qemu est bien présent : `ls`
3)  Exécutez le programme  : `./prog_qemu`
4)  si necessaire : `chmod +x prog_qemu`

# Collecte de donnée en temps réel

À chaque exécution du programme prog_qemu ou prog_debian, un parking est sélectionné aléatoirement dans la base de données. Nous traçons l'évolution du taux de disponibilité de ce parking à l'aide d'un histogramme. Ce graphique, nommé 'Nom_du_parking_evolution.png', est actualisé automatiquement toutes les 5 minutes, reflétant la disponibilité du parking à chaque intervalle.

## Demonstration : 

On voit 3 barre dans l'histogramme pour le fichier 'Parking Plaza_evolution.png' cela represente les 15 dernieres minutes. 18% correspond au taux de disponibilité du parking Plaza puis diminu a 17% un peu plus tard.
![evolution_parking2](https://github.com/raph5640/OpenData/assets/140059828/00e31736-e266-46cf-b759-224261717948)



