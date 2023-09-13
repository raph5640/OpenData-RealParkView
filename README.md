# OpenData

Sujet choisi : Disponibilité temps réel des parkings MEL
https://www.data.gouv.fr/fr/datasets/disponibilite-temps-reel-des-parkings-mel/

Deux fichiers sont proposés .json et .csv. 

1) Utilisation de l'URL stable pour le fichier .json : 
utiliser cette URL : https://opendata.lillemetropole.fr//explore/dataset/disponibilite-parkings/download?format=json&timezone=Europe/Berlin&use_labels_for_header=false

2) Téléchargement du fichier à partir de l'URL: 
On va utiliser curl ou des bibliothèques C++ comme libcurl pour télécharger le fichier CSV

3) Parser le fichier .JSON
On parse le fichier pour extraire les données necessaires 

4) Génération du graphique histogramme
On génére un graphique a partir des données récupéré dans une structure avec la bibliothèque GD.

## OBJECTIF : 

histogramme : calculer le pourcentage de disponibilité de chaque parking
![parking](https://github.com/raph5640/OpenData/assets/140059828/4dc396e7-e0dd-45f5-9912-b56439c1378f)


## Execution :
### Debian compilation :
`g++ -o prog_debian main.cpp -lgd -ljsoncpp -I/home/raphael/json/include`

### QEMU Compilation (buildroot)
1) faire un `make xconfig`
2) ajouter la bibliotheque json-for-modern-cpp : BR2_PACKAGE_JSON_FOR_MODERN_CPP
3) ajouter la bibliotheque gd : BR2_PACKAGE_GD
4) Assurez-vous que Enable C++ support est sélectionné/coché.

5) compilation buildroot : `~/buildroot-2023.08/output/host/bin/aarch64-buildroot-linux-gnu-g++ ~/OpenData/main.cpp -o ~/OpenData/prog_qemu -lgd -lstdc++fs`

6) Transfert du prog_emu compilé sur la machine debian vers la machine buildroot avec la commande a l'intérieur de la machine buildroot : `scp raphael@10.0.3.15:/home/raphael/OpenData/prog_qemu /root/`

#### Une fois dans la machine buildroot et le prog_emu transférer faire ceci :

7)  Allez dans le répertoire /root/: `cd /root/`
8)  Listez les fichiers pour voir si prog_qemu est bien présent : `ls`
9)  Exécutez le programme  : `./prog_qemu`
10)  si necessaire : `chmod +x prog_qemu`

