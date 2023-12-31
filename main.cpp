/**
 *
 * @file main.cpp
 * @brief Programme principal pour la gestion et la visualisation des données de disponibilité des parkings.
 * @author Raphael De Oliveira
 */
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "datamanager.h"
#include "histogram.h"
using namespace std;
const char* dirName = "Images_PNG";
const char* dataDirName = "Data_parking";
struct stat info;

/**
 * @brief Crée un répertoire s'il n'existe pas.
 * @param dirname Nom du répertoire à créer.
 * @return Retourne 'true' si le répertoire est créé ou s'il existe déjà, 'false' sinon.
 */
bool createDirectoryIfNotExists(const char* dirname) {
    if (stat(dirname, &info) != 0) {
        cout << "Création du répertoire : " << dirname << endl;
        if (mkdir(dirname, 0755) == 0) {  //0755 est le mode d'accès standard pour un répertoire
            return true;  //Répertoire créé avec succès
        } else {
            cerr << "Erreur lors de la création du répertoire : " << dirname << endl;
            return false;  //Échec de création du répertoire
        }
    } else if (info.st_mode & S_IFDIR) {  // On vérifi que c'est bien un répertoire
        cout << "Le répertoire " << dirname << " existe déjà." << endl;
        return true;  //Le répertoire existe déjà
    } else {
        cerr << "Erreur: " << dirname << " n'est pas un répertoire." << endl;
        return false;  //Le chemin n'est pas un répertoire
    }
}

int main() {
    DataManager dataManager;
    Histogram histogram;
    //Création du répertoire "Images_PNG" pour stocker les images générées par le programme
    createDirectoryIfNotExists(dirName);
    createDirectoryIfNotExists(dataDirName);

    while (true) {
        dataManager.collectData();

        //Obtention les noms des parkings depuis DataManager
        vector<string> noms = dataManager.getNoms();

        // Appele la fonction pour créer un histogramme d'évolution pour chaque parking
        for (const string& nom : noms) {
            //Construction du chemin complet du fichier JSON pour ce parking
            string jsonFilePath = string(dataDirName) + "/" + nom + ".json";

            //On vérifie si le fichier JSON existe
            if (access(jsonFilePath.c_str(), F_OK) != -1) {
                string histogramFilename = "Images_PNG/histogramme_evolution_" + nom + ".png";
                Histogram::createEvolutionHistogramFromJSON(histogramFilename, jsonFilePath);
            } else {
                cerr << "Le fichier JSON pour le parking '" << nom << "' n'existe pas." << endl;
            }
        }
        Histogram::createHistogram("Images_PNG/Taux_dispo_actuel_TOUT_parkings.png");
        histogram.showTerminalHistogram();
        histogram.showGeneratedImagesHTML();
        //On attend 10 minutes avant la prochaine collecte
        for (int i = 600; i > 0; --i) {
            // Calculer les minutes et les secondes restantes
            int minutes = i / 60;
            int seconds = i % 60;

            //Efface la ligne précédente dans le terminal (pour que le décompte soit sur la même ligne)
            cout << "\rProchaine collecte dans " << minutes << " minutes " << seconds << " secondes...    " << flush;

            //Attendre une seconde avant d'afficher le décompte suivant
            this_thread::sleep_for(std::chrono::seconds(1));
        }
        cout << endl;
    }

    return 0;
}
