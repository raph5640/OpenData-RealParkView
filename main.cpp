#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include "datamanager.h"
#include "histogram.h"
// Auteur : Raphael De Oliveira
using namespace std;
const char* dirName = "Images_PNG";
const char* dataDirName = "Data_parking";
struct stat info;

// Fonction pour créer un répertoire s'il n'existe pas
bool createDirectoryIfNotExists(const char* dirname) {
    if (stat(dirname, &info) != 0) {
        cout << "Création du répertoire : " << dirname << endl;
        if (mkdir(dirname, 0755) == 0) {  // 0755 est le mode d'accès standard pour un répertoire
            return true;  // Répertoire créé avec succès
        } else {
            cerr << "Erreur lors de la création du répertoire : " << dirname << endl;
            return false;  // Échec de création du répertoire
        }
    } else if (info.st_mode & S_IFDIR) {  // Vérifie que c'est bien un répertoire
        cout << "Le répertoire " << dirname << " existe déjà." << endl;
        return true;  // Le répertoire existe déjà
    } else {
        cerr << "Erreur: " << dirname << " n'est pas un répertoire." << endl;
        return false;  // Le chemin n'est pas un répertoire
    }
}

int main() {
    DataManager dataManager;

    // Création du répertoire "Images_PNG" pour stocker les images générées par le programme
    createDirectoryIfNotExists(dirName);
    createDirectoryIfNotExists(dataDirName);

    while (true) {
        dataManager.collectData();

        // Obtention les noms des parkings depuis DataManager
        vector<string> noms = dataManager.getNoms();

        // Appele la fonction pour créer un histogramme d'évolution pour chaque parking
        for (const string& nom : noms) {
            // Construction du chemin complet du fichier JSON pour ce parking
            string jsonFilePath = string(dataDirName) + "/" + nom + ".json";

            // On vérifie si le fichier JSON existe
            if (access(jsonFilePath.c_str(), F_OK) != -1) {
                string histogramFilename = "Images_PNG/histogramme_evolution_" + nom + ".png";
                Histogram::createEvolutionHistogramFromJSON(histogramFilename, jsonFilePath);
            } else {
                cerr << "Le fichier JSON pour le parking '" << nom << "' n'existe pas." << endl;
            }
        }

        // On attend 10 minutes avant la prochaine collecte
        this_thread::sleep_for(std::chrono::seconds(600));
    }

    return 0;
}
